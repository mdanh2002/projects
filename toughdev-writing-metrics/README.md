# ToughDev Writing Metrics

A reproducible analysis project for studying how the writing style of [ToughDev](https://www.toughdev.com) changed from 2008 to 2026.

This repository accompanies the article:

**What the Metrics Remember: Almost Two Decades of Technical Writing on ToughDev**  
https://www.toughdev.com/content/2026/06/what-the-metrics-remember-almost-two-decades-of-technical-writing-on-toughdev/

The project crawls public ToughDev article pages, extracts article and paragraph text, calculates deterministic writing metrics, uses a local LLM to classify first-person paragraphs, and generates CSV files, Markdown excerpts, and charts.

The main question is simple:

> Did ToughDev change from short technical notes into something closer to technical memoir?

The script does not decide that by itself. It only points to years, paragraphs, and patterns worth reading more carefully.

## Dataset summary

The current run produced:

```text
Articles scanned: 357
Year range: 2008–2026
Total words scanned: 323,028
Candidate first-person paragraphs: 2,165
AI-classified personal-experience paragraphs: 543
Parse errors: 0
```

Some headline results:

```text
2008 median article length: 342 words
2025 median article length: 1,471 words

2008 personal-experience paragraphs per 1,000 words: 0.148
2025 personal-experience paragraphs per 1,000 words: 3.411
2026 personal-experience paragraphs per 1,000 words: 4.319

Personal-experience category breakdown:
debugging:              278
other:                  103
project_history:         62
nostalgia:               37
hardware:                31
reverse_engineering:     30
```

The strongest conclusion from the current run is:

> ToughDev did not move from technical writing to personal writing. It moved from technical notes to technical evidence with a visible narrator.

## Repository contents

```text
scrape_toughdev.py              Crawl public ToughDev article pages
analyze_toughdev.py             Deterministic article and paragraph analysis
analyze_toughdev_ollama.py      Local LLM classification using Ollama
study_toughdev_results.py       Interactive utility for tables, charts, excerpts
README.md                       This file
```

Generated output is written under:

```text
output/toughdev_audit/
```

Typical generated files:

```text
summary.json
metrics_by_year.csv
metrics_by_article.csv
candidate_paragraphs.csv
ai_classifications.csv
paragraph_style_metrics.csv
literary_metrics_by_year.csv
blog_pack/*.md
charts/*.png
```

## Requirements

Python 3.10+ is recommended.

The deterministic analysis requires:

```text
requests
beautifulsoup4
readability-lxml
pandas
matplotlib
tabulate
```

The AI-assisted classification step requires [Ollama](https://ollama.com/).

The current run used:

```text
Ollama
qwen2.5:3b
```

## Installation

Clone the repository:

```bash
git clone https://github.com/YOUR-GITHUB-USERNAME/toughdev-writing-metrics.git
cd toughdev-writing-metrics
```

Create a virtual environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Install Python dependencies:

```bash
pip install requests beautifulsoup4 readability-lxml pandas matplotlib tabulate
```

Install Ollama separately, then pull the model:

```bash
ollama pull qwen2.5:3b
```

Make sure Ollama is running:

```bash
ollama serve
```

If Ollama is already running as a background service, this command may report that the port is already in use. That is fine.

## Quick start

Run the full workflow in four steps:

```bash
# 1. Crawl public ToughDev article pages
python3 scrape_toughdev.py \
  --start-year 2008 \
  --end-year 2026 \
  --out data/toughdev_html

# 2. Run deterministic analysis
python3 analyze_toughdev.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit

# 3. Run local LLM paragraph classification
python3 analyze_toughdev_ollama.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit \
  --model qwen2.5:3b

# 4. Generate tables, excerpts, and charts
python3 study_toughdev_results.py --option 10
```

If your local copy of `analyze_toughdev_ollama.py` has slightly different command-line options, check them with:

```bash
python3 analyze_toughdev_ollama.py --help
```

## Step 1: Crawl ToughDev

```bash
python3 scrape_toughdev.py \
  --start-year 2008 \
  --end-year 2026 \
  --out data/toughdev_html
```

This downloads public ToughDev article HTML into:

```text
data/toughdev_html/
```

The crawler uses the public site rather than the WordPress database. This is intentional. It means the analysis sees roughly what a normal reader, crawler, or search tool would see, including some of the same messiness.

The crawler skips files that have already been downloaded, so it can be resumed.

## Step 2: Run deterministic analysis

```bash
python3 analyze_toughdev.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit
```

This produces the first set of CSV and JSON files:

```text
output/toughdev_audit/summary.json
output/toughdev_audit/metrics_by_year.csv
output/toughdev_audit/metrics_by_article.csv
output/toughdev_audit/candidate_paragraphs.csv
```

The deterministic pass calculates metrics such as:

```text
article count
word count
paragraph count
median article length
first-person candidate paragraphs
technical-density score
narrative markers
sensory words
rough narrative-prose score
```

It also extracts first-person candidate paragraphs for later classification.

## Step 3: Run local LLM classification

Pull the local model:

```bash
ollama pull qwen2.5:3b
```

Then run:

```bash
python3 analyze_toughdev_ollama.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit \
  --model qwen2.5:3b
```

The classifier reads candidate first-person paragraphs and labels them into rough categories:

```text
debugging
project_history
hardware
reverse_engineering
nostalgia
other
none
```

The main output is:

```text
output/toughdev_audit/ai_classifications.csv
```

This step uses the LLM as a local classifier, not as a writer. The model is used to help decide whether a first-person paragraph is actually personal experience and what kind of experience it represents.

The current run classified 543 paragraphs as personal experience out of 2,165 first-person candidates.

## Step 4: Explore the results

Run the interactive utility:

```bash
python3 study_toughdev_results.py
```

Menu:

```text
1. General summary
2. Main yearly metrics table
3. Personal-experience metrics
4. Personal-experience example paragraphs
5. Most literary / creative paragraphs
6. Highly technical articles and paragraphs
7. Most literary / creative articles
8. Search paragraphs
9. Generate charts as PNG
10. Generate blog pack markdown + charts
11. Literary metrics by year
0. Exit
```

Useful one-shot commands:

```bash
python3 study_toughdev_results.py --option 1
python3 study_toughdev_results.py --option 2
python3 study_toughdev_results.py --option 3
python3 study_toughdev_results.py --option 5
python3 study_toughdev_results.py --option 7
python3 study_toughdev_results.py --option 9
python3 study_toughdev_results.py --option 11
python3 study_toughdev_results.py --option 10
```

To regenerate paragraph-level style metrics:

```bash
python3 study_toughdev_results.py --regen-paragraphs
```

## Recommended commands for reproducing the article

To reproduce the main numbers and outputs used in the article, run:

```bash
python3 scrape_toughdev.py \
  --start-year 2008 \
  --end-year 2026 \
  --out data/toughdev_html

python3 analyze_toughdev.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit

ollama pull qwen2.5:3b

python3 analyze_toughdev_ollama.py \
  --html-root data/toughdev_html/articles \
  --out output/toughdev_audit \
  --model qwen2.5:3b

python3 study_toughdev_results.py --option 1
python3 study_toughdev_results.py --option 2
python3 study_toughdev_results.py --option 3
python3 study_toughdev_results.py --option 5
python3 study_toughdev_results.py --option 9
python3 study_toughdev_results.py --option 11
python3 study_toughdev_results.py --option 10
```

After this, check:

```text
output/toughdev_audit/summary.json
output/toughdev_audit/metrics_by_year.csv
output/toughdev_audit/metrics_by_article.csv
output/toughdev_audit/ai_classifications.csv
output/toughdev_audit/paragraph_style_metrics.csv
output/toughdev_audit/literary_metrics_by_year.csv
output/toughdev_audit/blog_pack/
output/toughdev_audit/charts/
```

## Main output files

### `summary.json`

High-level summary of the run.

Example values from the current run:

```text
articles_scanned: 357
total_words_scanned: 323028
candidate_first_person_paragraphs: 2165
ai_personal_experience_paragraphs: 543
parse_errors: 0
```

### `metrics_by_year.csv`

Yearly summary table.

Contains columns such as:

```text
year
article_count
total_words
median_article_words
median_literary_creativity_score
mean_literary_creativity_score
median_technical_density_score
ai_classified_personal_experience_paragraphs
ai_personal_experience_per_1000_words
top_ai_category
```

### `metrics_by_article.csv`

Per-article metrics.

Useful for finding unusually long, technical, or narrative-heavy articles.

### `candidate_paragraphs.csv`

First-person candidate paragraphs extracted before LLM classification.

### `ai_classifications.csv`

LLM classification output.

Contains paragraph-level labels such as:

```text
personal_experience
category
confidence
paragraph
title
url
year
```

### `paragraph_style_metrics.csv`

Paragraph-level writing-style metrics.

Useful for finding high-scoring narrative or technical paragraphs.

### `literary_metrics_by_year.csv`

Yearly paragraph-level narrative metrics.

Despite the filename, this should not be read as a true literary-quality score. A better interpretation is:

```text
narrative technical prose signal
```

or:

```text
technical memoir signal
```

### `blog_pack/`

Markdown files useful for writing the article.

Examples:

```text
01_summary.md
02_yearly_table.md
03_personal_experience.md
05_most_literary_paragraphs.md
07_most_literary_articles.md
10_blog_pack.md
11_literary_by_year.md
```

### `charts/`

Generated PNG charts.

Examples:

```text
median_article_words.png
personal_experience_per_1000_words.png
personal_experience_categories.png
literary_creativity_score.png
technical_density_score.png
literary_70_paragraphs_by_year.png
literary_80_paragraphs_by_year.png
memoir_like_paragraphs_by_year.png
```

## How the metrics should be interpreted

Some metrics are straightforward:

```text
word count
article count
median article length
paragraph count
personal-experience paragraph count
```

Others are proxies.

The column currently called:

```text
literary_creativity_score
```

is not a measure of literary quality.

The script does not know what literature is. It only measures signals such as:

```text
narrative markers
sensory words
figurative markers
quoted speech
punctuation variety
lexical diversity
lower pure-code density
```

A better name would be:

```text
narrative technical prose score
```

The score is a sieve, not a judge. It helps find paragraphs worth reading manually.

Debugging has a plot too, so some ordinary troubleshooting paragraphs score highly even when they are not literary in any serious sense.

## Relationship to the older Geocities experiment

This project is in the same spirit as an older ToughDev article:

**Making good sense of the 1TB Yahoo Geocities data archive**  
https://www.toughdev.com/content/2018/11/making-good-sense-of-the-1tb-yahoo-geocities-data-archive/

The older method used C#, LINQ, regular expressions, dictionaries, and hand-made keyword scoring. Cheerful words added points, depressing words subtracted points, and the score helped surface interesting paragraphs from millions of extracted paragraphs.

This project uses newer tools:

```text
Python
pandas
matplotlib
Ollama
qwen2.5:3b
```

But the spirit is mostly the same:

> Let the maths tell us what needs to be looked at, but keep the final judgement with the author.

A score can find a paragraph. It cannot decide whether the paragraph matters.

## Limitations

Known limitations:

```text
Paragraph extraction may include boilerplate, quotes, ratings, or sharing text.
The local LLM classification is an estimate, not ground truth.
No manually labelled validation set was used.
The narrative score is a proxy, not a measure of literary quality.
Short years such as 2026 should not be over-interpreted.
Some high-scoring paragraphs are false positives.
Quoted text can score highly even when it is not my own prose.
```

The goal is not to produce an objective ranking of writing quality.

The goal is to make a long-running technical archive easier to inspect.

## Troubleshooting

### `ModuleNotFoundError`

Install the dependencies again inside the virtual environment:

```bash
source .venv/bin/activate
pip install requests beautifulsoup4 readability-lxml pandas matplotlib tabulate
```

### Ollama connection error

Make sure Ollama is running:

```bash
ollama serve
```

Then check the model is installed:

```bash
ollama list
```

Pull the model if needed:

```bash
ollama pull qwen2.5:3b
```

### Missing `paragraph_style_metrics.csv`

Regenerate it:

```bash
python3 study_toughdev_results.py --regen-paragraphs
```

### Charts not generated

Run:

```bash
python3 study_toughdev_results.py --option 9
```

Charts should appear under:

```text
output/toughdev_audit/charts/
```

### Markdown blog pack not generated

Run:

```bash
python3 study_toughdev_results.py --option 10
```

Markdown files should appear under:

```text
output/toughdev_audit/blog_pack/
```

## Suggested article interpretation

The early blog was mostly a notebook. The later blog became more of a lab diary.

At first I mostly wrote down what worked.

Later, I started writing down why I kept trying.

## License

Add your preferred license here.

Suggested:

```text
MIT License for code
CC BY 4.0 for generated article text and charts
```
