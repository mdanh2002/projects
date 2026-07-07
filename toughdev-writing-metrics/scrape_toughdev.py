#!/usr/bin/env python3
"""
Scrape public ToughDev article pages from yearly archive pages.

Starts from:
  https://www.toughdev.com/content/2008/
  ...
  https://www.toughdev.com/content/2026/

Features:
- Crawls year, month, and paginated archive links.
- Finds individual article links.
- Downloads article HTML into a folder.
- Prints progress as it goes.
- Bypasses SSL verification by default.
- Resume-friendly: skips article URLs already recorded in index.jsonl if the HTML file exists.

Usage:
  python scrape_toughdev.py --start-year 2008 --end-year 2026 --out data/toughdev_html
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import time
from pathlib import Path
from urllib.parse import urljoin, urlparse, urldefrag

import requests
from bs4 import BeautifulSoup

try:
    import urllib3

    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
except Exception:
    pass


ARTICLE_RE = re.compile(
    r"^/content/(?P<year>\d{4})/(?P<month>\d{2})/"
    r"(?:(?P<day>\d{2})/)?(?P<slug>[^/?#]+)/?$"
)

YEAR_ARCHIVE_RE = re.compile(
    r"^/content/(?P<year>\d{4})/(?:page/\d+/?)?$"
)

MONTH_ARCHIVE_RE = re.compile(
    r"^/content/(?P<year>\d{4})/(?P<month>\d{2})/?$"
)


def normalize_url(url: str) -> str:
    """Remove fragments/query strings and normalize directory-style URLs."""
    url, _fragment = urldefrag(url)
    parsed = urlparse(url)

    path = parsed.path
    if not path.endswith("/") and "." not in Path(path).name:
        path += "/"

    return parsed._replace(path=path, query="").geturl()


def is_toughdev_url(url: str) -> bool:
    host = urlparse(url).netloc.lower()
    return host in {"www.toughdev.com", "toughdev.com"}


def is_article_url(url: str, year: int | None = None) -> bool:
    parsed = urlparse(url)
    match = ARTICLE_RE.match(parsed.path)

    if not match:
        return False

    if year is not None and int(match.group("year")) != year:
        return False

    return True


def is_archive_url(url: str, year: int) -> bool:
    parsed = urlparse(url)
    path = parsed.path

    match = YEAR_ARCHIVE_RE.match(path)
    if match and int(match.group("year")) == year:
        return True

    match = MONTH_ARCHIVE_RE.match(path)
    if match and int(match.group("year")) == year:
        return True

    return False


def short_hash(text: str, length: int = 10) -> str:
    return hashlib.sha1(text.encode("utf-8")).hexdigest()[:length]


def safe_slug_from_url(url: str) -> str:
    parsed = urlparse(url)
    match = ARTICLE_RE.match(parsed.path)

    if match:
        slug = match.group("slug")
    else:
        slug = parsed.path.strip("/") or "page"

    slug = re.sub(r"[^a-zA-Z0-9_-]+", "-", slug)
    return slug[:90] or "page"


def get_year_from_url(url: str) -> int | None:
    parsed = urlparse(url)
    match = re.match(r"^/content/(\d{4})/", parsed.path)
    return int(match.group(1)) if match else None


def fetch(
    session: requests.Session,
    url: str,
    timeout: int,
    verify_ssl: bool,
    retries: int,
) -> requests.Response:
    last_error: Exception | None = None

    for attempt in range(1, retries + 1):
        try:
            response = session.get(url, timeout=timeout, verify=verify_ssl)
            response.raise_for_status()
            return response
        except Exception as exc:
            last_error = exc
            print(
                f"[warn] fetch failed {attempt}/{retries}: {url} :: {exc}",
                flush=True,
            )
            time.sleep(min(2 * attempt, 8))

    raise RuntimeError(f"failed after {retries} attempts: {url}") from last_error


def extract_links(base_url: str, html: str) -> set[str]:
    soup = BeautifulSoup(html, "html.parser")
    links: set[str] = set()

    for anchor in soup.find_all("a", href=True):
        href = anchor.get("href", "").strip()
        if not href:
            continue

        absolute = normalize_url(urljoin(base_url, href))
        if is_toughdev_url(absolute):
            links.add(absolute)

    return links


def extract_title(html: str) -> str:
    soup = BeautifulSoup(html, "html.parser")

    h1 = soup.find("h1")
    if h1:
        title = h1.get_text(" ", strip=True)
        if title:
            return title

    if soup.title:
        return soup.title.get_text(" ", strip=True)

    return ""


def write_jsonl(path: Path, obj: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    with path.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(obj, ensure_ascii=False) + "\n")


def load_downloaded_urls(index_file: Path) -> set[str]:
    downloaded: set[str] = set()

    if not index_file.exists():
        return downloaded

    with index_file.open("r", encoding="utf-8") as handle:
        for line in handle:
            try:
                row = json.loads(line)
            except Exception:
                continue

            url = row.get("url")
            html_path = row.get("html_path")

            if url and html_path and Path(html_path).exists():
                downloaded.add(url)

    return downloaded


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", default="https://www.toughdev.com/content")
    parser.add_argument("--start-year", type=int, default=2008)
    parser.add_argument("--end-year", type=int, default=2026)
    parser.add_argument("--out", default="data/toughdev_html")
    parser.add_argument("--delay", type=float, default=0.7)
    parser.add_argument("--timeout", type=int, default=30)
    parser.add_argument("--retries", type=int, default=3)
    parser.add_argument(
        "--verify-ssl",
        action="store_true",
        help="Enable SSL verification. Default is disabled.",
    )
    parser.add_argument("--max-archive-pages-per-year", type=int, default=250)

    args = parser.parse_args()

    out_dir = Path(args.out)
    article_dir = out_dir / "articles"
    archive_dir = out_dir / "archives"
    index_file = out_dir / "index.jsonl"
    error_file = out_dir / "errors.jsonl"

    article_dir.mkdir(parents=True, exist_ok=True)
    archive_dir.mkdir(parents=True, exist_ok=True)

    session = requests.Session()
    session.headers.update(
        {
            "User-Agent": "ToughDevWritingAudit/0.1 (+https://www.toughdev.com)",
            "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
        }
    )

    downloaded_urls = load_downloaded_urls(index_file)
    all_article_urls: set[str] = set()

    print(f"[init] output folder: {out_dir}", flush=True)
    print(f"[init] already downloaded articles: {len(downloaded_urls)}", flush=True)
    print(f"[init] SSL verification enabled: {args.verify_ssl}", flush=True)

    for year in range(args.start_year, args.end_year + 1):
        print(f"\n=== YEAR {year} ===", flush=True)

        start_url = normalize_url(f"{args.base.rstrip('/')}/{year}/")
        archive_queue = [start_url]
        seen_archives: set[str] = set()

        while archive_queue and len(seen_archives) < args.max_archive_pages_per_year:
            archive_url = archive_queue.pop(0)

            if archive_url in seen_archives:
                continue

            seen_archives.add(archive_url)
            print(f"[archive] {archive_url}", flush=True)

            try:
                response = fetch(
                    session=session,
                    url=archive_url,
                    timeout=args.timeout,
                    verify_ssl=args.verify_ssl,
                    retries=args.retries,
                )
            except Exception as exc:
                print(f"    [archive error] {exc}", flush=True)
                write_jsonl(
                    error_file,
                    {
                        "type": "archive",
                        "url": archive_url,
                        "error": str(exc),
                    },
                )
                continue

            archive_year_dir = archive_dir / str(year)
            archive_year_dir.mkdir(parents=True, exist_ok=True)

            archive_path = archive_year_dir / f"archive-{short_hash(archive_url)}.html"
            archive_path.write_text(response.text, encoding="utf-8", errors="replace")

            links = extract_links(archive_url, response.text)

            article_links = {url for url in links if is_article_url(url, year=year)}
            archive_links = {
                url
                for url in links
                if is_archive_url(url, year=year) and url not in seen_archives
            }

            before_count = len(all_article_urls)
            all_article_urls.update(article_links)
            after_count = len(all_article_urls)

            for link in sorted(archive_links):
                if link not in archive_queue:
                    archive_queue.append(link)

            print(
                f"    article_links={len(article_links)} "
                f"new_articles={after_count - before_count} "
                f"total_articles={after_count} "
                f"queued_archives={len(archive_queue)}",
                flush=True,
            )

            time.sleep(args.delay)

    print(f"\n=== DOWNLOADING ARTICLES: {len(all_article_urls)} discovered ===", flush=True)

    for index, url in enumerate(sorted(all_article_urls), start=1):
        year = get_year_from_url(url) or 0
        year_dir = article_dir / str(year)
        year_dir.mkdir(parents=True, exist_ok=True)

        slug = safe_slug_from_url(url)
        filename = f"{slug}-{short_hash(url)}.html"
        html_path = year_dir / filename

        if url in downloaded_urls and html_path.exists():
            print(f"[{index}/{len(all_article_urls)}] skip existing {url}", flush=True)
            continue

        print(f"[{index}/{len(all_article_urls)}] download {url}", flush=True)

        try:
            response = fetch(
                session=session,
                url=url,
                timeout=args.timeout,
                verify_ssl=args.verify_ssl,
                retries=args.retries,
            )

            html_path.write_text(response.text, encoding="utf-8", errors="replace")
            title = extract_title(response.text)

            write_jsonl(
                index_file,
                {
                    "url": url,
                    "year": year,
                    "title": title,
                    "html_path": str(html_path),
                    "status_code": response.status_code,
                    "bytes": len(response.content),
                    "downloaded_at": time.strftime("%Y-%m-%d %H:%M:%S"),
                },
            )

            print(f"    saved {html_path} title={title!r}", flush=True)

        except Exception as exc:
            print(f"    [article error] {exc}", flush=True)
            write_jsonl(
                error_file,
                {
                    "type": "article",
                    "url": url,
                    "error": str(exc),
                },
            )

        time.sleep(args.delay)

    print("\nDone.", flush=True)
    print(f"Index:  {index_file}", flush=True)
    print(f"Errors: {error_file}", flush=True)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
