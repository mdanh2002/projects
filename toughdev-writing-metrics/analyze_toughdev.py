#!/usr/bin/env python3
"""
Analyze downloaded ToughDev HTML files and optionally classify candidate paragraphs
with Ollama.

This version adds deterministic writing-style metrics, including a
literary_creativity_score that estimates how much the writing resembles
narrative/literary prose rather than pure technical documentation.

Important note:
  The literary_creativity_score is a heuristic proxy, not ground truth.
  It uses measurable signals such as figurative-language markers, sensory
  words, narrative markers, lexical diversity, punctuation variety, and
  technical density. It should be described as an estimate in the article.

Deterministic-only analysis:

  python analyze_toughdev_ollama.py \
    --html-root data/toughdev_html/articles \
    --out output/toughdev_audit

Small Ollama test:

  python analyze_toughdev_ollama.py \
    --html-root data/toughdev_html/articles \
    --out output/toughdev_audit \
    --classify \
    --ollama-model qwen2.5:3b \
    --classify-limit 20 \
    --clear-cache

Full Ollama classification:

  python analyze_toughdev_ollama.py \
    --html-root data/toughdev_html/articles \
    --out output/toughdev_audit \
    --classify \
    --ollama-model qwen2.5:3b \
    --clear-cache
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import re
import statistics
import sys
import time
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from urllib.parse import urlparse

import requests
from bs4 import BeautifulSoup


FIRST_PERSON_RE = re.compile(
    r"\b("
    r"I|I'm|I've|I'd|I'll|me|my|mine|myself|"
    r"when I|I used|I built|I found|I noticed|I remember|I tried|I wrote|"
    r"I fixed|I debugged|in my experience|from my experience|I learned|"
    r"I discovered|I measured|I tested|my aim|my goal"
    r")\b",
    re.IGNORECASE,
)

WORD_RE = re.compile(r"[A-Za-z0-9]+(?:[-'][A-Za-z0-9]+)?")
SENTENCE_RE = re.compile(r"[.!?]+")
YEAR_RE = re.compile(r"/content/(\d{4})/")

# These lists are deliberately small and transparent. The goal is not to make a
# perfect literary classifier; it is to produce a reproducible proxy metric.
FIGURATIVE_MARKERS = [
    "like a",
    "like an",
    "as if",
    "as though",
    "reminds me",
    "reminded me",
    "feels like",
    "felt like",
    "sounds like",
    "looked like",
    "seems like",
    "almost like",
    "a kind of",
    "sort of",
    "rather than",
    "in a way",
]

NARRATIVE_MARKERS = [
    "i remember",
    "i still remember",
    "back then",
    "at the time",
    "one day",
    "years ago",
    "when i",
    "after i",
    "before i",
    "eventually",
    "finally",
    "suddenly",
    "later",
    "the first time",
    "my first",
    "i decided",
    "i wanted",
    "i tried",
    "i found",
    "i noticed",
    "i learned",
    "i realized",
]

SENSORY_WORDS = {
    "bright",
    "dark",
    "dim",
    "glowing",
    "color",
    "colour",
    "blue",
    "green",
    "red",
    "yellow",
    "black",
    "white",
    "grey",
    "gray",
    "warm",
    "cold",
    "hot",
    "cool",
    "tiny",
    "large",
    "huge",
    "small",
    "heavy",
    "light",
    "soft",
    "hard",
    "rough",
    "smooth",
    "sharp",
    "quiet",
    "silent",
    "loud",
    "noisy",
    "noise",
    "sound",
    "smell",
    "taste",
    "touch",
    "feel",
    "felt",
    "visible",
    "invisible",
    "clear",
    "blurred",
    "blurry",
}

CREATIVE_ADJECTIVE_SUFFIXES = (
    "ful",
    "ous",
    "ive",
    "less",
    "able",
    "ible",
    "ic",
    "ical",
    "ish",
    "ary",
    "ory",
)

TECHNICAL_TERMS = {
    "api",
    "apis",
    "array",
    "async",
    "bios",
    "bit",
    "bits",
    "buffer",
    "byte",
    "bytes",
    "cache",
    "class",
    "client",
    "code",
    "command",
    "compile",
    "compiler",
    "config",
    "configuration",
    "cpu",
    "cuda",
    "database",
    "debug",
    "debugging",
    "device",
    "driver",
    "endpoint",
    "error",
    "file",
    "firmware",
    "function",
    "gpu",
    "hardware",
    "hex",
    "http",
    "https",
    "interface",
    "kernel",
    "library",
    "linux",
    "memory",
    "method",
    "module",
    "network",
    "object",
    "packet",
    "parameter",
    "pointer",
    "port",
    "protocol",
    "python",
    "register",
    "request",
    "response",
    "rom",
    "script",
    "server",
    "socket",
    "software",
    "source",
    "sql",
    "stack",
    "string",
    "tcp",
    "thread",
    "usb",
    "variable",
    "version",
    "windows",
}

CODELIKE_RE = re.compile(
    r"(\b[A-Za-z_][A-Za-z0-9_]*\s*\(|==|!=|<=|>=|::|->|=>|;|\{\}|\{.*\}|</?[a-zA-Z][^>]*>|\\x[0-9a-fA-F]{2})"
)


@dataclass
class Article:
    path: Path
    title: str
    url: str
    year: int | None
    text: str
    paragraphs: list[str]
    html: str
    soup: BeautifulSoup


def sha1(text: str) -> str:
    return hashlib.sha1(text.encode("utf-8")).hexdigest()


def clamp(value: float, low: float = 0.0, high: float = 100.0) -> float:
    return max(low, min(high, value))


def scaled_per_1000(count: int | float, total_words: int) -> float:
    return round(float(count) / max(total_words, 1) * 1000, 3)


def count_phrase_markers(text_lower: str, markers: list[str]) -> int:
    return sum(text_lower.count(marker) for marker in markers)


def count_syllables(word: str) -> int:
    word = word.lower()
    vowels = "aeiouy"
    count = 0
    previous_is_vowel = False

    for char in word:
        is_vowel = char in vowels
        if is_vowel and not previous_is_vowel:
            count += 1
        previous_is_vowel = is_vowel

    if word.endswith("e") and count > 1:
        count -= 1

    return max(count, 1)


def flesch_reading_ease(text: str) -> float | None:
    words = WORD_RE.findall(text)

    if not words:
        return None

    sentence_count = max(len(SENTENCE_RE.findall(text)), 1)
    syllable_count = sum(count_syllables(word) for word in words)

    return (
        206.835
        - 1.015 * (len(words) / sentence_count)
        - 84.6 * (syllable_count / len(words))
    )


def clean_soup(soup: BeautifulSoup) -> None:
    for tag in soup(
        [
            "script",
            "style",
            "noscript",
            "svg",
            "form",
            "header",
            "footer",
            "nav",
            "aside",
        ]
    ):
        tag.decompose()


def pick_content_node(soup: BeautifulSoup):
    selectors = [
        "article",
        ".entry-content",
        ".post-content",
        ".td-post-content",
        ".content",
        "main",
    ]

    for selector in selectors:
        node = soup.select_one(selector)
        if node and len(node.get_text(" ", strip=True)) > 200:
            return node

    return soup.body if soup.body else soup


def extract_url_from_html(soup: BeautifulSoup, path: Path) -> str:
    canonical = soup.find("link", rel=lambda value: value and "canonical" in value)

    if canonical and canonical.get("href"):
        return canonical["href"].strip()

    og_url = soup.find("meta", property="og:url")

    if og_url and og_url.get("content"):
        return og_url["content"].strip()

    return str(path)


def extract_title(soup: BeautifulSoup) -> str:
    h1 = soup.find("h1")

    if h1:
        title = h1.get_text(" ", strip=True)
        if title:
            return title

    if soup.title:
        return soup.title.get_text(" ", strip=True)

    return ""


def extract_year(url: str, path: Path) -> int | None:
    match = YEAR_RE.search(url)

    if match:
        return int(match.group(1))

    for part in path.parts:
        if re.fullmatch(r"\d{4}", part):
            return int(part)

    return None


def extract_paragraphs(content_node) -> list[str]:
    paragraphs: list[str] = []

    for node in content_node.find_all(["p", "li", "blockquote"]):
        text = node.get_text(" ", strip=True)
        text = re.sub(r"\s+", " ", text)

        if len(text) >= 30:
            paragraphs.append(text)

    if not paragraphs:
        raw = content_node.get_text("\n", strip=True)

        for line in raw.splitlines():
            line = re.sub(r"\s+", " ", line).strip()

            if len(line) >= 30:
                paragraphs.append(line)

    return paragraphs


def load_article(path: Path) -> Article:
    html = path.read_text(encoding="utf-8", errors="replace")
    soup = BeautifulSoup(html, "html.parser")
    clean_soup(soup)

    url = extract_url_from_html(soup, path)
    title = extract_title(soup)
    year = extract_year(url, path)

    content_node = pick_content_node(soup)
    paragraphs = extract_paragraphs(content_node)
    text = "\n\n".join(paragraphs)

    return Article(
        path=path,
        title=title,
        url=url,
        year=year,
        text=text,
        paragraphs=paragraphs,
        html=html,
        soup=soup,
    )


def writing_style_metrics(text: str, paragraphs: list[str]) -> dict[str, Any]:
    words_raw = WORD_RE.findall(text)
    words = [word.lower() for word in words_raw]
    total_words = len(words)
    text_lower = text.lower()

    if total_words == 0:
        return {
            "lexical_diversity": 0,
            "figurative_marker_count": 0,
            "figurative_markers_per_1000_words": 0,
            "narrative_marker_count": 0,
            "narrative_markers_per_1000_words": 0,
            "sensory_word_count": 0,
            "sensory_words_per_1000_words": 0,
            "creative_modifier_count": 0,
            "creative_modifiers_per_1000_words": 0,
            "question_count": 0,
            "question_marks_per_1000_words": 0,
            "exclamation_count": 0,
            "exclamation_marks_per_1000_words": 0,
            "quote_count": 0,
            "quotes_per_1000_words": 0,
            "punctuation_variety_score": 0,
            "technical_term_count": 0,
            "technical_terms_per_1000_words": 0,
            "code_like_line_count": 0,
            "code_like_lines_per_1000_words": 0,
            "technical_density_score": 0,
            "literary_creativity_score": 0,
        }

    unique_words = len(set(words))
    lexical_diversity = unique_words / total_words

    figurative_marker_count = count_phrase_markers(text_lower, FIGURATIVE_MARKERS)
    narrative_marker_count = count_phrase_markers(text_lower, NARRATIVE_MARKERS)
    sensory_word_count = sum(1 for word in words if word in SENSORY_WORDS)

    creative_modifier_count = 0
    for word in words:
        if len(word) >= 5 and (word.endswith("ly") or word.endswith(CREATIVE_ADJECTIVE_SUFFIXES)):
            creative_modifier_count += 1

    question_count = text.count("?")
    exclamation_count = text.count("!")
    quote_count = text.count('"') + text.count("“") + text.count("”") + text.count("‘") + text.count("’")

    punctuation_types = 0
    for punct in [",", ";", ":", "—", "-", "?", "!", "(", ")", '"', "“", "”"]:
        if punct in text:
            punctuation_types += 1
    punctuation_variety_score = round(punctuation_types / 12 * 100, 3)

    technical_term_count = sum(1 for word in words if word in TECHNICAL_TERMS)
    code_like_line_count = 0
    for paragraph in paragraphs:
        if CODELIKE_RE.search(paragraph):
            code_like_line_count += 1

    technical_terms_per_1000 = technical_term_count / total_words * 1000
    code_like_lines_per_1000 = code_like_line_count / total_words * 1000

    # Technical density is intentionally capped so one code-heavy article does not
    # dominate the 0-100 scale.
    technical_density_score = clamp(
        technical_terms_per_1000 * 1.4 + code_like_lines_per_1000 * 12,
        0,
        100,
    )

    # A reproducible proxy for literary / creative prose. The score rises with
    # narrative, sensory, figurative, and rhetorically varied writing, but falls
    # when the article is dominated by technical vocabulary or code-like text.
    literary_raw = (
        min(30, lexical_diversity * 50)
        + min(20, figurative_marker_count / total_words * 1000 * 8)
        + min(20, narrative_marker_count / total_words * 1000 * 5)
        + min(15, sensory_word_count / total_words * 1000 * 1.8)
        + min(10, creative_modifier_count / total_words * 1000 * 0.8)
        + min(8, question_count / total_words * 1000 * 1.8)
        + min(5, exclamation_count / total_words * 1000 * 2.0)
        + min(8, quote_count / total_words * 1000 * 0.6)
        + min(10, punctuation_variety_score / 10)
        - min(25, technical_density_score * 0.25)
    )

    literary_creativity_score = clamp(literary_raw, 0, 100)

    return {
        "lexical_diversity": round(lexical_diversity, 4),
        "figurative_marker_count": figurative_marker_count,
        "figurative_markers_per_1000_words": scaled_per_1000(figurative_marker_count, total_words),
        "narrative_marker_count": narrative_marker_count,
        "narrative_markers_per_1000_words": scaled_per_1000(narrative_marker_count, total_words),
        "sensory_word_count": sensory_word_count,
        "sensory_words_per_1000_words": scaled_per_1000(sensory_word_count, total_words),
        "creative_modifier_count": creative_modifier_count,
        "creative_modifiers_per_1000_words": scaled_per_1000(creative_modifier_count, total_words),
        "question_count": question_count,
        "question_marks_per_1000_words": scaled_per_1000(question_count, total_words),
        "exclamation_count": exclamation_count,
        "exclamation_marks_per_1000_words": scaled_per_1000(exclamation_count, total_words),
        "quote_count": quote_count,
        "quotes_per_1000_words": scaled_per_1000(quote_count, total_words),
        "punctuation_variety_score": punctuation_variety_score,
        "technical_term_count": technical_term_count,
        "technical_terms_per_1000_words": round(technical_terms_per_1000, 3),
        "code_like_line_count": code_like_line_count,
        "code_like_lines_per_1000_words": round(code_like_lines_per_1000, 3),
        "technical_density_score": round(technical_density_score, 3),
        "literary_creativity_score": round(literary_creativity_score, 3),
    }


def article_metrics(article: Article) -> dict[str, Any]:
    words = WORD_RE.findall(article.text)
    sentences = SENTENCE_RE.findall(article.text)

    content_node = pick_content_node(article.soup)

    links = content_node.find_all("a", href=True)
    internal_links = 0
    external_links = 0

    for anchor in links:
        href = anchor.get("href", "")
        parsed = urlparse(href)

        if not parsed.netloc or "toughdev.com" in parsed.netloc:
            internal_links += 1
        else:
            external_links += 1

    code_blocks = len(content_node.find_all(["code", "pre"]))
    images = len(content_node.find_all("img"))
    headings = len(content_node.find_all(["h2", "h3", "h4"]))

    paragraph_lengths = [
        len(WORD_RE.findall(paragraph)) for paragraph in article.paragraphs
    ]

    first_person_candidates = [
        paragraph for paragraph in article.paragraphs if FIRST_PERSON_RE.search(paragraph)
    ]

    readability = flesch_reading_ease(article.text)
    style = writing_style_metrics(article.text, article.paragraphs)

    metrics = {
        "path": str(article.path),
        "url": article.url,
        "title": article.title,
        "year": article.year,
        "word_count": len(words),
        "sentence_count": len(sentences),
        "paragraph_count": len(article.paragraphs),
        "median_paragraph_words": (
            statistics.median(paragraph_lengths) if paragraph_lengths else 0
        ),
        "mean_paragraph_words": (
            round(statistics.mean(paragraph_lengths), 2) if paragraph_lengths else 0
        ),
        "very_long_paragraphs_120w": sum(
            1 for paragraph_length in paragraph_lengths if paragraph_length >= 120
        ),
        "heading_count": headings,
        "headings_per_1000_words": round(
            headings / max(len(words), 1) * 1000,
            3,
        ),
        "code_block_count": code_blocks,
        "image_count": images,
        "internal_link_count": internal_links,
        "external_link_count": external_links,
        "first_person_candidate_paragraphs": len(first_person_candidates),
        "first_person_candidates_per_1000_words": round(
            len(first_person_candidates) / max(len(words), 1) * 1000,
            3,
        ),
        "article_has_first_person_candidate": bool(first_person_candidates),
        "flesch_reading_ease": (
            round(readability, 3) if readability is not None else None
        ),
    }

    metrics.update(style)
    return metrics


def write_csv(path: Path, rows: list[dict[str, Any]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    if not rows:
        path.write_text("", encoding="utf-8")
        return

    keys: list[str] = []
    seen: set[str] = set()

    for row in rows:
        for key in row.keys():
            if key not in seen:
                keys.append(key)
                seen.add(key)

    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=keys)
        writer.writeheader()
        writer.writerows(rows)


def load_cache(path: Path) -> dict[str, dict[str, Any]]:
    if not path.exists():
        return {}

    cache: dict[str, dict[str, Any]] = {}

    with path.open("r", encoding="utf-8") as handle:
        for line in handle:
            try:
                row = json.loads(line)
            except Exception:
                continue

            paragraph_hash = row.get("paragraph_hash")
            if paragraph_hash:
                cache[paragraph_hash] = row

    return cache


def append_cache(path: Path, row: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    with path.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(row, ensure_ascii=False) + "\n")


def normalize_bool(value: Any) -> bool | None:
    if isinstance(value, bool):
        return value

    if value is None:
        return None

    text = str(value).strip().lower()

    if text in {"true", "yes", "1"}:
        return True

    if text in {"false", "no", "0"}:
        return False

    return None


def guess_category(text: str) -> str:
    if any(word in text for word in ["debug", "bug", "fix", "error", "crash"]):
        return "debugging"

    if any(word in text for word in ["hardware", "board", "pcb", "chip", "device", "circuit"]):
        return "hardware"

    if any(word in text for word in ["remember", "old", "childhood", "nostalgia"]):
        return "nostalgia"

    if any(word in text for word in ["reverse", "firmware", "dump", "disassemble", "patch"]):
        return "reverse_engineering"

    if any(word in text for word in ["project", "built", "wrote", "implemented", "developed"]):
        return "project_history"

    return "other"


def parse_jsonish(text: str) -> dict[str, Any]:
    original = text or ""
    text = original.strip()

    if not text:
        return {
            "personal_experience": None,
            "category": "parse_error",
            "confidence": 0.0,
            "raw": original,
        }

    text = text.replace("```json", "").replace("```", "").strip()

    start = text.find("{")
    end = text.rfind("}")

    if start >= 0 and end > start:
        text = text[start : end + 1]

    try:
        data = json.loads(text)
    except Exception:
        lower = text.lower()

        if "personal_experience" in lower and "true" in lower:
            return {
                "personal_experience": True,
                "category": guess_category(lower),
                "confidence": 0.5,
                "raw": original,
            }

        if "personal_experience" in lower and "false" in lower:
            return {
                "personal_experience": False,
                "category": "none",
                "confidence": 0.5,
                "raw": original,
            }

        return {
            "personal_experience": None,
            "category": "parse_error",
            "confidence": 0.0,
            "raw": original,
        }

    personal_experience = normalize_bool(data.get("personal_experience"))

    category = str(data.get("category", "none")).strip().lower()
    allowed = {
        "debugging",
        "hardware",
        "nostalgia",
        "reverse_engineering",
        "project_history",
        "other",
        "none",
    }

    if category not in allowed:
        category = "other" if personal_experience else "none"

    try:
        confidence = float(data.get("confidence", 0.0) or 0.0)
    except Exception:
        confidence = 0.0

    confidence = max(0.0, min(1.0, confidence))

    return {
        "personal_experience": personal_experience,
        "category": category,
        "confidence": confidence,
        "raw": original,
    }


class OllamaClassifier:
    def __init__(
        self,
        model: str,
        url: str,
        timeout: int,
        use_json_format: bool = False,
        debug_raw: bool = False,
    ):
        self.model = model
        self.url = url.rstrip("/")
        self.timeout = timeout
        self.use_json_format = use_json_format
        self.debug_raw = debug_raw

    def classify(self, paragraph: str) -> dict[str, Any]:
        prompt = f"""
Return one JSON object only. Do not use Markdown. Do not explain.

You are classifying paragraphs from a technical blog.

Task:
Decide whether this paragraph contains the author's personal technical experience.

IMPORTANT:
If the paragraph says "I", "my", "I am using", "I built", "I wrote", "I modified",
"my aim", "I found", "I tested", "I tried", "I debugged", or similar, it usually
counts as personal_experience=true.

Mark personal_experience=true if the author personally:
- used a device or software
- built or modified software
- wrote code, firmware, scripts, tools, or articles
- tested, measured, repaired, configured, or debugged something
- reverse engineered something
- describes their own project goal or reason for modifying something
- remembers something from their own past

Mark personal_experience=false only when the paragraph is:
- pure source code
- generic instructions
- reference links
- quoted third-party material
- navigation/download text
- explanation with no author involvement

Examples:

Paragraph: "As I am using both a PocketPC and Smartphone, my aim is to improve the application so that it can run on both devices."
Answer: {{"personal_experience": true, "category": "project_history", "confidence": 0.95}}

Paragraph: "Private Sub ListViewContacts_DrawItem(ByVal sender As Object...)"
Answer: {{"personal_experience": false, "category": "none", "confidence": 0.95}}

Paragraph: "You will need to download the original application and install it."
Answer: {{"personal_experience": false, "category": "none", "confidence": 0.95}}

Paragraph: "I found that the firmware checks this value before enabling RTSP."
Answer: {{"personal_experience": true, "category": "reverse_engineering", "confidence": 0.95}}

Required JSON schema:
{{
  "personal_experience": true,
  "category": "project_history",
  "confidence": 0.95
}}

Allowed category values:
debugging, hardware, nostalgia, reverse_engineering, project_history, other, none

Paragraph:
\"\"\"{paragraph[:2500]}\"\"\"
""".strip()

        payload: dict[str, Any] = {
            "model": self.model,
            "prompt": prompt,
            "stream": False,
            "options": {
                "temperature": 0,
                "num_ctx": 4096,
                "num_predict": 120,
            },
        }

        if self.use_json_format:
            payload["format"] = "json"

        response = requests.post(
            f"{self.url}/api/generate",
            json=payload,
            timeout=self.timeout,
        )
        response.raise_for_status()

        data = response.json()
        raw_text = data.get("response", "")

        if self.debug_raw:
            print("    [raw]", repr(raw_text[:500]), flush=True)

        return parse_jsonish(raw_text)


def aggregate_by_year(
    article_rows: list[dict[str, Any]],
    classification_rows: list[dict[str, Any]] | None = None,
) -> list[dict[str, Any]]:
    by_year: dict[int, list[dict[str, Any]]] = defaultdict(list)

    for row in article_rows:
        year = row.get("year")
        if year:
            by_year[int(year)].append(row)

    classified_by_year: dict[int, list[dict[str, Any]]] = defaultdict(list)

    if classification_rows:
        for row in classification_rows:
            year = row.get("year")
            if year:
                classified_by_year[int(year)].append(row)

    output: list[dict[str, Any]] = []

    for year in sorted(by_year):
        rows = by_year[year]

        word_counts = [int(row["word_count"]) for row in rows]
        paragraph_lengths = [float(row["median_paragraph_words"]) for row in rows]
        readability_scores = [
            float(row["flesch_reading_ease"])
            for row in rows
            if row.get("flesch_reading_ease") not in (None, "")
        ]
        literary_scores = [float(row["literary_creativity_score"]) for row in rows]
        technical_density_scores = [float(row["technical_density_score"]) for row in rows]
        lexical_diversities = [float(row["lexical_diversity"]) for row in rows]

        total_words = sum(word_counts)
        total_candidates = sum(
            int(row["first_person_candidate_paragraphs"]) for row in rows
        )
        total_headings = sum(int(row["heading_count"]) for row in rows)

        year_row = {
            "year": year,
            "article_count": len(rows),
            "total_words": total_words,
            "median_article_words": (
                statistics.median(word_counts) if word_counts else 0
            ),
            "median_paragraph_words": (
                statistics.median(paragraph_lengths) if paragraph_lengths else 0
            ),
            "articles_with_first_person_candidate": sum(
                1 for row in rows if str(row["article_has_first_person_candidate"]).lower() == "true"
            ),
            "first_person_candidate_paragraphs": total_candidates,
            "first_person_candidates_per_1000_words": scaled_per_1000(total_candidates, total_words),
            "median_flesch_reading_ease": (
                round(statistics.median(readability_scores), 3)
                if readability_scores
                else None
            ),
            "total_headings": total_headings,
            "headings_per_1000_words": scaled_per_1000(total_headings, total_words),
            "total_code_blocks": sum(int(row["code_block_count"]) for row in rows),
            "total_images": sum(int(row["image_count"]) for row in rows),
            "total_external_links": sum(
                int(row["external_link_count"]) for row in rows
            ),
            "total_internal_links": sum(
                int(row["internal_link_count"]) for row in rows
            ),
            "very_long_paragraphs_120w": sum(
                int(row["very_long_paragraphs_120w"]) for row in rows
            ),
            "median_literary_creativity_score": round(statistics.median(literary_scores), 3),
            "mean_literary_creativity_score": round(statistics.mean(literary_scores), 3),
            "median_technical_density_score": round(statistics.median(technical_density_scores), 3),
            "mean_technical_density_score": round(statistics.mean(technical_density_scores), 3),
            "median_lexical_diversity": round(statistics.median(lexical_diversities), 4),
            "total_figurative_markers": sum(int(row["figurative_marker_count"]) for row in rows),
            "figurative_markers_per_1000_words": scaled_per_1000(
                sum(int(row["figurative_marker_count"]) for row in rows),
                total_words,
            ),
            "total_narrative_markers": sum(int(row["narrative_marker_count"]) for row in rows),
            "narrative_markers_per_1000_words": scaled_per_1000(
                sum(int(row["narrative_marker_count"]) for row in rows),
                total_words,
            ),
            "total_sensory_words": sum(int(row["sensory_word_count"]) for row in rows),
            "sensory_words_per_1000_words": scaled_per_1000(
                sum(int(row["sensory_word_count"]) for row in rows),
                total_words,
            ),
        }

        classified = classified_by_year.get(year, [])

        if classified:
            valid_classified = [
                row
                for row in classified
                if str(row.get("category", "")).lower() != "parse_error"
            ]

            personal = [
                row
                for row in classified
                if str(row.get("personal_experience", "")).lower() == "true"
            ]

            categories = Counter(row.get("category", "unknown") for row in personal)

            year_row.update(
                {
                    "ai_classified_candidate_paragraphs": len(classified),
                    "ai_valid_classified_candidate_paragraphs": len(valid_classified),
                    "ai_parse_error_paragraphs": len(classified) - len(valid_classified),
                    "ai_classified_personal_experience_paragraphs": len(personal),
                    "ai_personal_experience_per_1000_words": scaled_per_1000(len(personal), total_words),
                    "top_ai_category": (
                        categories.most_common(1)[0][0] if categories else None
                    ),
                }
            )
        else:
            year_row.update(
                {
                    "ai_classified_candidate_paragraphs": 0,
                    "ai_valid_classified_candidate_paragraphs": 0,
                    "ai_parse_error_paragraphs": 0,
                    "ai_classified_personal_experience_paragraphs": 0,
                    "ai_personal_experience_per_1000_words": 0,
                    "top_ai_category": None,
                }
            )

        output.append(year_row)

    return output


def check_ollama(url: str, timeout: int) -> None:
    try:
        response = requests.get(f"{url.rstrip('/')}/api/tags", timeout=timeout)
        response.raise_for_status()
    except Exception as exc:
        raise RuntimeError(
            f"Could not connect to Ollama at {url}. "
            f"Start Ollama first, then retry. Error: {exc}"
        ) from exc


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--html-root", default="data/toughdev_html/articles")
    parser.add_argument("--out", default="output/toughdev_audit")

    parser.add_argument("--classify", action="store_true")
    parser.add_argument("--ollama-model", default="qwen2.5:3b")
    parser.add_argument("--ollama-url", default="http://localhost:11434")
    parser.add_argument("--ollama-timeout", type=int, default=180)
    parser.add_argument(
        "--ollama-json-format",
        action="store_true",
        help="Ask Ollama for JSON mode. Default is off because some models return empty output.",
    )
    parser.add_argument(
        "--debug-raw",
        action="store_true",
        help="Print raw Ollama output during classification.",
    )

    parser.add_argument(
        "--limit",
        type=int,
        default=0,
        help="Limit articles for testing. 0 means no limit.",
    )
    parser.add_argument(
        "--classify-limit",
        type=int,
        default=0,
        help="Limit candidate paragraphs for testing. 0 means no limit.",
    )
    parser.add_argument(
        "--clear-cache",
        action="store_true",
        help="Delete the Ollama classification cache before running.",
    )
    parser.add_argument(
        "--sleep",
        type=float,
        default=0.0,
        help="Optional delay between Ollama requests.",
    )

    args = parser.parse_args()

    html_root = Path(args.html_root)
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    if not html_root.exists():
        print(f"[error] html root not found: {html_root}", file=sys.stderr)
        return 1

    html_files = sorted(html_root.rglob("*.html"))

    if args.limit:
        html_files = html_files[: args.limit]

    print(f"[load] html root: {html_root}", flush=True)
    print(f"[load] html files found: {len(html_files)}", flush=True)

    article_rows: list[dict[str, Any]] = []
    candidate_rows: list[dict[str, Any]] = []

    for index, path in enumerate(html_files, start=1):
        print(f"[article {index}/{len(html_files)}] {path}", flush=True)

        try:
            article = load_article(path)
        except Exception as exc:
            print(f"    [parse error] {exc}", flush=True)
            continue

        metrics = article_metrics(article)
        article_rows.append(metrics)

        for paragraph_index, paragraph in enumerate(article.paragraphs):
            if FIRST_PERSON_RE.search(paragraph):
                candidate_rows.append(
                    {
                        "url": article.url,
                        "title": article.title,
                        "year": article.year,
                        "paragraph_index": paragraph_index,
                        "paragraph_hash": sha1(paragraph),
                        "paragraph": paragraph,
                    }
                )

    metrics_by_article_path = out_dir / "metrics_by_article.csv"
    candidate_paragraphs_path = out_dir / "candidate_paragraphs.csv"

    write_csv(metrics_by_article_path, article_rows)
    write_csv(candidate_paragraphs_path, candidate_rows)

    print(f"[write] {metrics_by_article_path} rows={len(article_rows)}", flush=True)
    print(f"[write] {candidate_paragraphs_path} rows={len(candidate_rows)}", flush=True)

    classification_rows: list[dict[str, Any]] = []

    if args.classify:
        check_ollama(args.ollama_url, timeout=10)

        cache_path = out_dir / "classification_cache_ollama.jsonl"

        if args.clear_cache and cache_path.exists():
            cache_path.unlink()
            print(f"[cache] deleted {cache_path}", flush=True)

        classifier = OllamaClassifier(
            model=args.ollama_model,
            url=args.ollama_url,
            timeout=args.ollama_timeout,
            use_json_format=args.ollama_json_format,
            debug_raw=args.debug_raw,
        )

        cache = load_cache(cache_path)

        rows_to_classify = candidate_rows

        if args.classify_limit:
            rows_to_classify = rows_to_classify[: args.classify_limit]

        print(f"[classify] model: {args.ollama_model}", flush=True)
        print(f"[classify] ollama url: {args.ollama_url}", flush=True)
        print(f"[classify] json format: {args.ollama_json_format}", flush=True)
        print(f"[classify] candidate paragraphs: {len(rows_to_classify)}", flush=True)
        print(f"[classify] cache entries: {len(cache)}", flush=True)

        for index, row in enumerate(rows_to_classify, start=1):
            paragraph_hash = row["paragraph_hash"]

            if paragraph_hash in cache:
                result = cache[paragraph_hash]
                print(
                    f"[classify {index}/{len(rows_to_classify)}] "
                    f"cache {paragraph_hash[:8]} "
                    f"{result.get('personal_experience')} "
                    f"{result.get('category')}",
                    flush=True,
                )
            else:
                print(
                    f"[classify {index}/{len(rows_to_classify)}] "
                    f"ollama {paragraph_hash[:8]}",
                    flush=True,
                )

                try:
                    classified = classifier.classify(row["paragraph"])
                except Exception as exc:
                    print(f"    [ollama error] {exc}", flush=True)
                    classified = {
                        "personal_experience": None,
                        "category": "ollama_error",
                        "confidence": 0.0,
                        "raw": str(exc),
                    }

                result = {
                    **row,
                    **classified,
                }

                append_cache(cache_path, result)

                if args.sleep:
                    time.sleep(args.sleep)

            classification_rows.append(result)

        ai_classifications_path = out_dir / "ai_classifications.csv"
        write_csv(ai_classifications_path, classification_rows)

        print(
            f"[write] {ai_classifications_path} rows={len(classification_rows)}",
            flush=True,
        )

        parse_errors = sum(
            1 for row in classification_rows if row.get("category") == "parse_error"
        )
        personal_count = sum(
            1
            for row in classification_rows
            if str(row.get("personal_experience", "")).lower() == "true"
        )

        print(f"[classify] personal_experience true: {personal_count}", flush=True)
        print(f"[classify] parse errors: {parse_errors}", flush=True)

    yearly_rows = aggregate_by_year(
        article_rows=article_rows,
        classification_rows=classification_rows if classification_rows else None,
    )

    metrics_by_year_path = out_dir / "metrics_by_year.csv"
    write_csv(metrics_by_year_path, yearly_rows)

    print(f"[write] {metrics_by_year_path} rows={len(yearly_rows)}", flush=True)

    summary = {
        "articles": len(article_rows),
        "candidate_paragraphs": len(candidate_rows),
        "classified_paragraphs": len(classification_rows),
        "years": [row["year"] for row in yearly_rows],
        "classifier": "ollama" if args.classify else None,
        "ollama_model": args.ollama_model if args.classify else None,
        "ollama_json_format": args.ollama_json_format if args.classify else None,
        "style_metric_note": (
            "literary_creativity_score is a deterministic heuristic proxy for narrative, "
            "sensory, figurative, and rhetorically varied prose relative to technical density. "
            "It is not a human literary judgment."
        ),
        "classification_note": (
            "AI classifications are estimates, not ground truth. "
            "No manually labelled validation set was used."
        ),
    }

    summary_path = out_dir / "summary.json"
    summary_path.write_text(
        json.dumps(summary, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )

    print(f"[write] {summary_path}", flush=True)
    print("\nDone.", flush=True)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
