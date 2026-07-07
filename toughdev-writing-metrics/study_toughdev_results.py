#!/usr/bin/env python3
"""
Interactive utility for studying ToughDev audit output.

Reads:
  output/toughdev_audit/metrics_by_year.csv
  output/toughdev_audit/metrics_by_article.csv
  output/toughdev_audit/ai_classifications.csv

Can also generate paragraph-level style metrics:
  output/toughdev_audit/paragraph_style_metrics.csv

Run:
  python study_toughdev_results.py

Optional:
  python study_toughdev_results.py --option 9
  python study_toughdev_results.py --option 11
  python study_toughdev_results.py --regen-paragraphs
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


OUT_DIR = Path("output/toughdev_audit")
HTML_ROOT = Path("data/toughdev_html/articles")
BLOG_PACK = OUT_DIR / "blog_pack"
CHART_DIR = OUT_DIR / "charts"


def load_csv(path: Path) -> pd.DataFrame:
    if not path.exists():
        raise FileNotFoundError(f"Missing file: {path}")
    return pd.read_csv(path)


def load_all() -> tuple[pd.DataFrame, pd.DataFrame, pd.DataFrame]:
    year = load_csv(OUT_DIR / "metrics_by_year.csv")
    article = load_csv(OUT_DIR / "metrics_by_article.csv")
    cls = load_csv(OUT_DIR / "ai_classifications.csv")
    return year, article, cls


def personal_rows(cls: pd.DataFrame) -> pd.DataFrame:
    return cls[cls["personal_experience"].astype(str).str.lower() == "true"].copy()


def print_title(text: str) -> None:
    print()
    print("=" * 120)
    print(text)
    print("=" * 120)


def save_md(name: str, text: str) -> Path:
    BLOG_PACK.mkdir(parents=True, exist_ok=True)
    path = BLOG_PACK / name
    path.write_text(text, encoding="utf-8")
    print(f"[write] {path}")
    return path


def df_to_md(df: pd.DataFrame) -> str:
    try:
        return df.to_markdown(index=False)
    except Exception:
        return df.to_string(index=False)


def series_to_md(series: pd.Series) -> str:
    try:
        return series.to_markdown()
    except Exception:
        return series.to_string()


def option_1_summary() -> None:
    year, article, cls = load_all()
    personal = personal_rows(cls)

    summary = {
        "articles_scanned": int(year["article_count"].sum()),
        "total_words_scanned": int(year["total_words"].sum()),
        "candidate_first_person_paragraphs": len(cls),
        "ai_personal_experience_paragraphs": len(personal),
        "parse_errors": int((cls["category"] == "parse_error").sum()),
        "personal_experience_rate_among_candidates_percent": round(
            len(personal) / max(len(cls), 1) * 100,
            2,
        ),
        "year_range": f"{int(year['year'].min())}-{int(year['year'].max())}",
        "median_article_words_overall": round(article["word_count"].median(), 2),
        "median_literary_creativity_score": round(
            article["literary_creativity_score"].median(),
            3,
        ),
        "median_technical_density_score": round(
            article["technical_density_score"].median(),
            3,
        ),
    }

    print_title("1. General summary")

    for key, value in summary.items():
        print(f"{key}: {value}")

    md = "# ToughDev audit summary\n\n"
    md += "\n".join(f"- **{key}**: {value}" for key, value in summary.items())
    md += "\n"

    save_md("01_summary.md", md)


def option_2_yearly_table() -> None:
    year, _, _ = load_all()

    print_title("2. Main yearly table")

    cols = [
        "year",
        "article_count",
        "total_words",
        "median_article_words",
        "median_literary_creativity_score",
        "mean_literary_creativity_score",
        "median_technical_density_score",
        "ai_classified_personal_experience_paragraphs",
        "ai_personal_experience_per_1000_words",
        "top_ai_category",
    ]

    cols = [col for col in cols if col in year.columns]
    df = year[cols].copy()

    print(df.to_string(index=False))

    md = "# Main yearly metrics\n\n"
    md += df_to_md(df)
    md += "\n"

    save_md("02_yearly_table.md", md)


def option_3_personal_experience() -> None:
    year, _, cls = load_all()
    personal = personal_rows(cls)

    print_title("3. Personal-experience metrics")

    cols = [
        "year",
        "article_count",
        "total_words",
        "ai_classified_personal_experience_paragraphs",
        "ai_personal_experience_per_1000_words",
        "top_ai_category",
    ]

    cols = [col for col in cols if col in year.columns]

    by_year = year[cols].sort_values(
        "ai_personal_experience_per_1000_words",
        ascending=False,
    )

    print("\nMost personal years:")
    print(by_year.to_string(index=False))

    print("\nCategory breakdown:")
    category_counts = personal["category"].value_counts()
    print(category_counts.to_string())

    pivot = pd.crosstab(personal["year"], personal["category"])

    print("\nCategory by year:")
    print(pivot.to_string())

    md = "# Personal-experience metrics\n\n"
    md += "## Most personal years\n\n"
    md += df_to_md(by_year)
    md += "\n\n## Category breakdown\n\n"
    md += series_to_md(category_counts)
    md += "\n\n## Category by year\n\n"
    md += df_to_md(pivot.reset_index())
    md += "\n"

    save_md("03_personal_experience.md", md)


def option_4_personal_examples() -> None:
    _, _, cls = load_all()
    personal = personal_rows(cls)

    print_title("4. Personal-experience example paragraphs")
    print(
        "Note: these are first-hand technical/project examples. "
        "They are not necessarily literary passages."
    )

    sample = personal.sample(min(20, len(personal)), random_state=42)

    lines = ["# Sample personal-experience paragraphs\n"]

    for i, (_, row) in enumerate(sample.iterrows(), start=1):
        block = (
            f"\n## {i}. {int(row['year'])} — {row['category']}\n\n"
            f"**{row['title']}**\n\n"
            f"{row['url']}\n\n"
            f"- Confidence: {row['confidence']}\n\n"
            f"> {str(row['paragraph']).replace(chr(10), ' ')}\n"
        )
        lines.append(block)

        print("=" * 120)
        print(
            f"{i}. {int(row['year'])} | "
            f"{row['category']} | confidence {row['confidence']}"
        )
        print(row["title"])
        print(row["url"])
        print()
        print(row["paragraph"])
        print()

    save_md("04_personal_examples.md", "\n".join(lines))


def import_analyzer():
    try:
        from analyze_toughdev_ollama import (  # type: ignore
            WORD_RE,
            load_article,
            sha1,
            writing_style_metrics,
        )

        return load_article, writing_style_metrics, WORD_RE, sha1
    except Exception:
        pass

    try:
        from analyze_toughdev import (  # type: ignore
            WORD_RE,
            load_article,
            sha1,
            writing_style_metrics,
        )

        return load_article, writing_style_metrics, WORD_RE, sha1
    except Exception as exc:
        raise RuntimeError(
            "Could not import paragraph style helpers. "
            "Make sure analyze_toughdev_ollama.py is in ~/scraper "
            "and contains writing_style_metrics()."
        ) from exc


def ensure_paragraph_style_metrics(force: bool = False) -> pd.DataFrame:
    path = OUT_DIR / "paragraph_style_metrics.csv"

    if path.exists() and not force:
        return pd.read_csv(path)

    print_title("Generating paragraph-level style metrics")

    load_article, writing_style_metrics, WORD_RE, sha1 = import_analyzer()

    classification_by_hash: dict[str, dict[str, object]] = {}
    cls_path = OUT_DIR / "ai_classifications.csv"

    if cls_path.exists():
        cls = pd.read_csv(cls_path)
        for _, row in cls.iterrows():
            classification_by_hash[str(row["paragraph_hash"])] = {
                "personal_experience": row.get("personal_experience", ""),
                "category": row.get("category", ""),
                "confidence": row.get("confidence", ""),
            }

    rows: list[dict[str, object]] = []
    html_files = sorted(HTML_ROOT.rglob("*.html"))

    for idx, html_path in enumerate(html_files, start=1):
        if idx % 25 == 0 or idx == 1 or idx == len(html_files):
            print(f"[paragraph metrics] {idx}/{len(html_files)} {html_path}")

        article = load_article(html_path)

        for paragraph_index, paragraph in enumerate(article.paragraphs):
            word_count = len(WORD_RE.findall(paragraph))

            if word_count < 45:
                continue

            paragraph_hash = sha1(paragraph)
            style = writing_style_metrics(paragraph, [paragraph])
            cls_info = classification_by_hash.get(paragraph_hash, {})

            rows.append(
                {
                    "year": article.year,
                    "title": article.title,
                    "url": article.url,
                    "paragraph_index": paragraph_index,
                    "paragraph_hash": paragraph_hash,
                    "word_count": word_count,
                    "literary_creativity_score": style["literary_creativity_score"],
                    "technical_density_score": style["technical_density_score"],
                    "lexical_diversity": style["lexical_diversity"],
                    "figurative_marker_count": style["figurative_marker_count"],
                    "narrative_marker_count": style["narrative_marker_count"],
                    "sensory_word_count": style["sensory_word_count"],
                    "creative_modifier_count": style["creative_modifier_count"],
                    "personal_experience": cls_info.get("personal_experience", ""),
                    "category": cls_info.get("category", ""),
                    "confidence": cls_info.get("confidence", ""),
                    "paragraph": paragraph,
                }
            )

    df = pd.DataFrame(rows)

    if not df.empty:
        df = df.sort_values(
            ["literary_creativity_score", "word_count"],
            ascending=[False, False],
        )

    df.to_csv(path, index=False)
    print(f"[write] {path} rows={len(df)}")

    return df


def literary_paragraph_flags(para: pd.DataFrame) -> pd.DataFrame:
    para = para.copy()

    para["is_literary_70"] = para["literary_creativity_score"] >= 70
    para["is_literary_80"] = para["literary_creativity_score"] >= 80
    para["is_literary_90"] = para["literary_creativity_score"] >= 90

    para["is_memoir_like"] = (
        (para["literary_creativity_score"] >= 70)
        & (para["technical_density_score"] <= 55)
        & (
            (para["narrative_marker_count"] >= 2)
            | (para["sensory_word_count"] >= 2)
            | (para["creative_modifier_count"] >= 2)
            | (para["category"].astype(str).str.lower() == "nostalgia")
        )
    )

    return para


def option_5_most_literary_paragraphs() -> None:
    df = ensure_paragraph_style_metrics()

    print_title("5. Most literary / creative paragraphs")

    top = df.sort_values(
        ["literary_creativity_score", "word_count"],
        ascending=[False, False],
    ).head(50)

    lines = ["# Most literary / creative paragraphs\n"]

    for rank, (_, row) in enumerate(top.iterrows(), start=1):
        print("=" * 120)
        print(
            f"Rank {rank} | {int(row['year'])} | "
            f"score {row['literary_creativity_score']} | "
            f"technical {row['technical_density_score']} | "
            f"words {int(row['word_count'])}"
        )
        print(row["title"])
        print(row["url"])
        print()
        print(row["paragraph"])
        print()

        lines.append(
            f"## {rank}. Score {row['literary_creativity_score']} "
            f"— {int(row['year'])}\n"
        )
        lines.append(f"**{row['title']}**\n")
        lines.append(f"{row['url']}\n")
        lines.append(f"- Technical density: {row['technical_density_score']}")
        lines.append(f"- Words: {int(row['word_count'])}")
        lines.append(f"- Paragraph index: {int(row['paragraph_index'])}\n")
        lines.append("> " + str(row["paragraph"]).replace("\n", " "))
        lines.append("")

    save_md("05_most_literary_paragraphs.md", "\n".join(lines))


def option_6_highly_technical() -> None:
    _, article, _ = load_all()
    para = ensure_paragraph_style_metrics()

    print_title("6. Highly technical writing")

    article_cols = [
        "year",
        "technical_density_score",
        "literary_creativity_score",
        "word_count",
        "title",
        "url",
    ]

    top_articles = article.sort_values(
        ["technical_density_score", "word_count"],
        ascending=[False, False],
    )[article_cols].head(30)

    print("\nMost technical articles:")
    print(top_articles.to_string(index=False, max_colwidth=100))

    top_paras = para.sort_values(
        ["technical_density_score", "word_count"],
        ascending=[False, False],
    ).head(30)

    lines = ["# Highly technical writing\n\n"]
    lines.append("## Most technical articles\n\n")
    lines.append(df_to_md(top_articles))
    lines.append("\n\n## Most technical paragraphs\n")

    print("\nMost technical paragraphs:")

    for rank, (_, row) in enumerate(top_paras.iterrows(), start=1):
        print("=" * 120)
        print(
            f"{rank}. {int(row['year'])} | "
            f"technical {row['technical_density_score']} | "
            f"literary {row['literary_creativity_score']}"
        )
        print(row["title"])
        print(row["url"])
        print()
        print(str(row["paragraph"])[:1200])
        print()

        lines.append(
            f"\n### {rank}. Technical {row['technical_density_score']} "
            f"— {int(row['year'])}\n"
        )
        lines.append(f"**{row['title']}**\n")
        lines.append(f"{row['url']}\n")
        lines.append("> " + str(row["paragraph"]).replace("\n", " ")[:1500])
        lines.append("")

    save_md("06_highly_technical.md", "\n".join(lines))


def option_7_most_literary_articles() -> None:
    _, article, _ = load_all()

    print_title("7. Most literary / creative articles")

    cols = [
        "year",
        "literary_creativity_score",
        "technical_density_score",
        "word_count",
        "title",
        "url",
    ]

    top = article.sort_values(
        ["literary_creativity_score", "word_count"],
        ascending=[False, False],
    )[cols].head(40)

    print(top.to_string(index=False, max_colwidth=120))

    md = "# Most literary / creative articles\n\n"
    md += df_to_md(top)
    md += "\n"

    save_md("07_most_literary_articles.md", md)


def option_8_keyword_search() -> None:
    para = ensure_paragraph_style_metrics()

    print_title("8. Search paragraphs")

    query = input("Search terms / regex: ").strip()

    if not query:
        print("Empty query.")
        return

    hits = para[
        para["paragraph"].str.contains(query, case=False, na=False, regex=True)
        | para["title"].str.contains(query, case=False, na=False, regex=True)
    ].copy()

    hits = hits.sort_values(
        ["literary_creativity_score", "word_count"],
        ascending=[False, False],
    )

    print(f"Hits: {len(hits)}")

    lines = [f"# Search results for `{query}`\n"]

    for rank, (_, row) in enumerate(hits.head(50).iterrows(), start=1):
        print("=" * 120)
        print(
            f"{rank}. {int(row['year'])} | "
            f"literary {row['literary_creativity_score']} | "
            f"technical {row['technical_density_score']} | "
            f"{row['category']}"
        )
        print(row["title"])
        print(row["url"])
        print()
        print(row["paragraph"])
        print()

        lines.append(
            f"## {rank}. {int(row['year'])} "
            f"— score {row['literary_creativity_score']}\n"
        )
        lines.append(f"**{row['title']}**\n")
        lines.append(f"{row['url']}\n")
        lines.append(f"- Technical density: {row['technical_density_score']}")
        lines.append(f"- Personal experience: {row['personal_experience']}")
        lines.append(f"- Category: {row['category']}\n")
        lines.append("> " + str(row["paragraph"]).replace("\n", " "))
        lines.append("")

    safe_query = "".join(c if c.isalnum() else "_" for c in query.lower())[:40]
    save_md(f"08_search_{safe_query}.md", "\n".join(lines))


def save_line_chart(
    df: pd.DataFrame,
    x: str,
    y: str,
    title: str,
    ylabel: str,
    filename: str,
) -> None:
    if y not in df.columns:
        print(f"[skip] missing column for chart: {y}")
        return

    CHART_DIR.mkdir(parents=True, exist_ok=True)

    plt.figure(figsize=(10, 5))
    plt.plot(df[x], df[y], marker="o")
    plt.title(title)
    plt.xlabel("Year")
    plt.ylabel(ylabel)
    plt.xticks(df[x], rotation=45)
    plt.tight_layout()

    path = CHART_DIR / filename
    plt.savefig(path, dpi=160)
    plt.close()

    print(f"[write] {path}")


def save_bar_chart(
    series: pd.Series,
    title: str,
    ylabel: str,
    filename: str,
) -> None:
    CHART_DIR.mkdir(parents=True, exist_ok=True)

    plt.figure(figsize=(10, 5))
    series.plot(kind="bar")
    plt.title(title)
    plt.xlabel("")
    plt.ylabel(ylabel)
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()

    path = CHART_DIR / filename
    plt.savefig(path, dpi=160)
    plt.close()

    print(f"[write] {path}")


def option_9_charts() -> None:
    year, _, cls = load_all()
    personal = personal_rows(cls)

    print_title("9. Generate charts")

    save_line_chart(
        year,
        "year",
        "median_article_words",
        "Median article length by year",
        "Words",
        "median_article_words.png",
    )

    save_line_chart(
        year,
        "year",
        "ai_personal_experience_per_1000_words",
        "Personal-experience paragraphs per 1,000 words",
        "Paragraphs per 1,000 words",
        "personal_experience_per_1000_words.png",
    )

    save_line_chart(
        year,
        "year",
        "median_literary_creativity_score",
        "Median literary / creative proxy score by year",
        "Score",
        "literary_creativity_score.png",
    )

    save_line_chart(
        year,
        "year",
        "median_technical_density_score",
        "Median technical-density score by year",
        "Score",
        "technical_density_score.png",
    )

    save_line_chart(
        year,
        "year",
        "narrative_markers_per_1000_words",
        "Narrative markers per 1,000 words",
        "Markers per 1,000 words",
        "narrative_markers_per_1000_words.png",
    )

    save_line_chart(
        year,
        "year",
        "sensory_words_per_1000_words",
        "Sensory words per 1,000 words",
        "Words per 1,000 words",
        "sensory_words_per_1000_words.png",
    )

    save_bar_chart(
        personal["category"].value_counts(),
        "Personal-experience category breakdown",
        "Paragraph count",
        "personal_experience_categories.png",
    )

    para = ensure_paragraph_style_metrics()
    para = literary_paragraph_flags(para)

    literary_70_by_year = para.groupby("year")["is_literary_70"].sum()
    literary_80_by_year = para.groupby("year")["is_literary_80"].sum()
    memoir_like_by_year = para.groupby("year")["is_memoir_like"].sum()

    save_bar_chart(
        literary_70_by_year,
        "Paragraphs with literary score >= 70 by year",
        "Paragraph count",
        "literary_70_paragraphs_by_year.png",
    )

    save_bar_chart(
        literary_80_by_year,
        "Paragraphs with literary score >= 80 by year",
        "Paragraph count",
        "literary_80_paragraphs_by_year.png",
    )

    save_bar_chart(
        memoir_like_by_year,
        "Memoir-like literary paragraphs by year",
        "Paragraph count",
        "memoir_like_paragraphs_by_year.png",
    )

    print("\nCharts saved in:")
    print(CHART_DIR)


def option_10_blog_pack() -> None:
    year, article, cls = load_all()
    personal = personal_rows(cls)
    para = ensure_paragraph_style_metrics()

    print_title("10. Generate blog pack")

    BLOG_PACK.mkdir(parents=True, exist_ok=True)

    summary_lines: list[str] = []
    summary_lines.append("# ToughDev audit blog pack\n")

    summary_lines.append("## Core numbers\n")
    summary_lines.append(f"- Articles scanned: {int(year['article_count'].sum())}")
    summary_lines.append(f"- Total words scanned: {int(year['total_words'].sum()):,}")
    summary_lines.append(f"- Candidate first-person paragraphs: {len(cls):,}")
    summary_lines.append(
        f"- AI-classified personal-experience paragraphs: {len(personal):,}"
    )
    summary_lines.append(f"- Parse errors: {int((cls['category'] == 'parse_error').sum())}")
    summary_lines.append(
        f"- Median article length: {round(article['word_count'].median(), 2)} words"
    )
    summary_lines.append(
        f"- Median literary/creative score: "
        f"{round(article['literary_creativity_score'].median(), 3)}"
    )
    summary_lines.append("")

    summary_lines.append("## Suggested thesis\n")
    summary_lines.append(
        "> ToughDev began as short, dense technical notes, code snippets, "
        "and reference posts. Over time, the articles became longer, "
        "more narrative, and more grounded in personal project history, "
        "while still remaining technically dense."
    )
    summary_lines.append("")

    yearly_cols = [
        "year",
        "article_count",
        "total_words",
        "median_article_words",
        "median_literary_creativity_score",
        "ai_classified_personal_experience_paragraphs",
        "ai_personal_experience_per_1000_words",
        "top_ai_category",
    ]

    yearly_cols = [col for col in yearly_cols if col in year.columns]

    summary_lines.append("## Main yearly table\n")
    summary_lines.append(df_to_md(year[yearly_cols]))
    summary_lines.append("")

    summary_lines.append("## Personal-experience categories\n")
    summary_lines.append(series_to_md(personal["category"].value_counts()))
    summary_lines.append("")

    summary_lines.append("## Most literary paragraphs\n")
    top_lit = para.sort_values(
        ["literary_creativity_score", "word_count"],
        ascending=[False, False],
    ).head(10)

    for rank, (_, row) in enumerate(top_lit.iterrows(), start=1):
        summary_lines.append(
            f"### {rank}. Score {row['literary_creativity_score']} "
            f"— {int(row['year'])}"
        )
        summary_lines.append(f"**{row['title']}**")
        summary_lines.append(f"{row['url']}")
        summary_lines.append("")
        summary_lines.append("> " + str(row["paragraph"]).replace("\n", " "))
        summary_lines.append("")

    summary_lines.append("## Strong personal-experience examples\n")
    sample = personal.sort_values("confidence", ascending=False).head(10)

    for rank, (_, row) in enumerate(sample.iterrows(), start=1):
        summary_lines.append(f"### {rank}. {int(row['year'])} — {row['category']}")
        summary_lines.append(f"**{row['title']}**")
        summary_lines.append(f"{row['url']}")
        summary_lines.append("")
        summary_lines.append("> " + str(row["paragraph"]).replace("\n", " "))
        summary_lines.append("")

    path = save_md("10_blog_pack.md", "\n".join(summary_lines))

    option_9_charts()

    print()
    print("Blog pack generated:")
    print(path)
    print(CHART_DIR)


def option_11_literary_by_year() -> None:
    year, _, _ = load_all()
    para = ensure_paragraph_style_metrics()
    para = literary_paragraph_flags(para)

    print_title("11. Literary metrics by year")

    article_cols = [
        "year",
        "article_count",
        "total_words",
        "median_article_words",
        "median_literary_creativity_score",
        "mean_literary_creativity_score",
        "median_technical_density_score",
        "mean_technical_density_score",
        "figurative_markers_per_1000_words",
        "narrative_markers_per_1000_words",
        "sensory_words_per_1000_words",
    ]

    article_cols = [col for col in article_cols if col in year.columns]
    article_year = year[article_cols].copy()

    para_year = (
        para.groupby("year")
        .agg(
            paragraph_count=("paragraph", "count"),
            median_paragraph_literary_score=("literary_creativity_score", "median"),
            mean_paragraph_literary_score=("literary_creativity_score", "mean"),
            max_paragraph_literary_score=("literary_creativity_score", "max"),
            median_paragraph_technical_density=("technical_density_score", "median"),
            literary_70_paragraphs=("is_literary_70", "sum"),
            literary_80_paragraphs=("is_literary_80", "sum"),
            literary_90_paragraphs=("is_literary_90", "sum"),
            memoir_like_paragraphs=("is_memoir_like", "sum"),
            total_narrative_markers=("narrative_marker_count", "sum"),
            total_sensory_words=("sensory_word_count", "sum"),
            total_figurative_markers=("figurative_marker_count", "sum"),
            total_creative_modifiers=("creative_modifier_count", "sum"),
        )
        .reset_index()
    )

    para_year["literary_70_pct"] = (
        para_year["literary_70_paragraphs"] / para_year["paragraph_count"] * 100
    ).round(2)

    para_year["literary_80_pct"] = (
        para_year["literary_80_paragraphs"] / para_year["paragraph_count"] * 100
    ).round(2)

    para_year["literary_90_pct"] = (
        para_year["literary_90_paragraphs"] / para_year["paragraph_count"] * 100
    ).round(2)

    para_year["memoir_like_pct"] = (
        para_year["memoir_like_paragraphs"] / para_year["paragraph_count"] * 100
    ).round(2)

    numeric_cols = [
        "median_paragraph_literary_score",
        "mean_paragraph_literary_score",
        "max_paragraph_literary_score",
        "median_paragraph_technical_density",
    ]

    for col in numeric_cols:
        para_year[col] = para_year[col].round(3)

    merged = article_year.merge(para_year, on="year", how="left")

    out_csv = OUT_DIR / "literary_metrics_by_year.csv"
    merged.to_csv(out_csv, index=False)
    print(f"[write] {out_csv}")

    display_cols = [
        "year",
        "article_count",
        "total_words",
        "median_literary_creativity_score",
        "mean_literary_creativity_score",
        "paragraph_count",
        "median_paragraph_literary_score",
        "max_paragraph_literary_score",
        "literary_70_paragraphs",
        "literary_80_paragraphs",
        "literary_90_paragraphs",
        "memoir_like_paragraphs",
        "memoir_like_pct",
    ]

    display_cols = [col for col in display_cols if col in merged.columns]

    print()
    print("Yearly literary metrics:")
    print(merged[display_cols].to_string(index=False))

    if para.empty:
        print("[warn] no paragraph metrics available")
        return

    best_idx = para.groupby("year")["literary_creativity_score"].idxmax()
    best = para.loc[best_idx].copy()
    best = best.sort_values("year")

    best_cols = [
        "year",
        "literary_creativity_score",
        "technical_density_score",
        "word_count",
        "title",
        "url",
        "paragraph",
    ]

    print()
    print_title("Best literary paragraph per year")

    for _, row in best[best_cols].iterrows():
        print("=" * 120)
        print(
            f"{int(row['year'])} | "
            f"literary {row['literary_creativity_score']} | "
            f"technical {row['technical_density_score']} | "
            f"words {int(row['word_count'])}"
        )
        print(row["title"])
        print(row["url"])
        print()
        print(row["paragraph"])
        print()

    lines = ["# Literary metrics by year\n"]
    lines.append("## Yearly literary metrics\n")
    lines.append(df_to_md(merged[display_cols]))
    lines.append("")

    lines.append("## Best literary paragraph per year\n")

    for _, row in best[best_cols].iterrows():
        lines.append(f"### {int(row['year'])} — score {row['literary_creativity_score']}\n")
        lines.append(f"**{row['title']}**\n")
        lines.append(f"{row['url']}\n")
        lines.append(f"- Technical density: {row['technical_density_score']}")
        lines.append(f"- Words: {int(row['word_count'])}\n")
        lines.append("> " + str(row["paragraph"]).replace("\n", " "))
        lines.append("")

    save_md("11_literary_by_year.md", "\n".join(lines))


def menu() -> None:
    print()
    print("ToughDev audit explorer")
    print("-" * 40)
    print("1. General summary")
    print("2. Main yearly metrics table")
    print("3. Personal-experience metrics")
    print("4. Personal-experience example paragraphs")
    print("5. Most literary / creative paragraphs")
    print("6. Highly technical articles and paragraphs")
    print("7. Most literary / creative articles")
    print("8. Search paragraphs")
    print("9. Generate charts as PNG")
    print("10. Generate blog pack markdown + charts")
    print("11. Literary metrics by year")
    print("0. Exit")
    print()


def run_option(option: int) -> None:
    if option == 1:
        option_1_summary()
    elif option == 2:
        option_2_yearly_table()
    elif option == 3:
        option_3_personal_experience()
    elif option == 4:
        option_4_personal_examples()
    elif option == 5:
        option_5_most_literary_paragraphs()
    elif option == 6:
        option_6_highly_technical()
    elif option == 7:
        option_7_most_literary_articles()
    elif option == 8:
        option_8_keyword_search()
    elif option == 9:
        option_9_charts()
    elif option == 10:
        option_10_blog_pack()
    elif option == 11:
        option_11_literary_by_year()
    elif option == 0:
        raise SystemExit(0)
    else:
        print("Unknown option.")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--option", type=int, default=None)
    parser.add_argument(
        "--regen-paragraphs",
        action="store_true",
        help="Regenerate paragraph_style_metrics.csv and exit.",
    )
    args = parser.parse_args()

    if args.regen_paragraphs:
        ensure_paragraph_style_metrics(force=True)
        return 0

    if args.option is not None:
        run_option(args.option)
        return 0

    while True:
        menu()
        raw = input("Choose option 0-11: ").strip()

        if not raw:
            continue

        try:
            option = int(raw)
        except ValueError:
            print("Please enter a number.")
            continue

        try:
            run_option(option)
        except KeyboardInterrupt:
            print("\nInterrupted.")
        except SystemExit:
            raise
        except Exception as exc:
            print(f"[error] {exc}", file=sys.stderr)

        input("\nPress Enter to continue...")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
