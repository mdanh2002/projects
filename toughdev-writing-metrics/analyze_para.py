from pathlib import Path
import csv
import pandas as pd

from analyze_toughdev import (
    load_article,
    writing_style_metrics,
    WORD_RE,
    sha1,
)

html_root = Path("data/toughdev_html/articles")
out_csv = Path("output/toughdev_audit/paragraph_style_metrics.csv")
out_md = Path("output/toughdev_audit/most_literary_paragraphs.md")

# Load AI personal-experience classification if available
classification_by_hash = {}
cls_path = Path("output/toughdev_audit/ai_classifications.csv")

if cls_path.exists():
    cls = pd.read_csv(cls_path)
    for _, row in cls.iterrows():
        classification_by_hash[str(row["paragraph_hash"])] = {
            "personal_experience": row.get("personal_experience", ""),
            "category": row.get("category", ""),
            "confidence": row.get("confidence", ""),
        }

rows = []

for path in sorted(html_root.rglob("*.html")):
    article = load_article(path)

    for paragraph_index, paragraph in enumerate(article.paragraphs):
        word_count = len(WORD_RE.findall(paragraph))

        # Ignore tiny fragments, menu text, and short captions.
        if word_count < 45:
            continue

        paragraph_hash = sha1(paragraph)
        style = writing_style_metrics(paragraph, [paragraph])
        cls_info = classification_by_hash.get(paragraph_hash, {})

        rows.append({
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
        })

df = pd.DataFrame(rows)
df = df.sort_values(
    ["literary_creativity_score", "word_count"],
    ascending=[False, False],
)

out_csv.parent.mkdir(parents=True, exist_ok=True)
df.to_csv(out_csv, index=False)

lines = []
lines.append("# Most Literary / Creative ToughDev Paragraphs\n")
lines.append(
    "Ranked using the deterministic `literary_creativity_score` proxy. "
    "This is not a human literary judgment; it highlights paragraphs with more narrative, sensory, figurative, varied prose and less pure code/reference density.\n"
)

for rank, (_, row) in enumerate(df.head(50).iterrows(), start=1):
    lines.append(f"## {rank}. Score {row['literary_creativity_score']} — {int(row['year'])}")
    lines.append("")
    lines.append(f"**{row['title']}**")
    lines.append("")
    lines.append(f"{row['url']}")
    lines.append("")
    lines.append(
        f"- Words: {int(row['word_count'])}"
        f"\n- Technical density: {row['technical_density_score']}"
        f"\n- Personal experience: {row['personal_experience']}"
        f"\n- Category: {row['category']}"
    )
    lines.append("")
    lines.append("> " + str(row["paragraph"]).replace("\n", " "))
    lines.append("")

out_md.write_text("\n".join(lines), encoding="utf-8")

print("Wrote:", out_csv)
print("Wrote:", out_md)
print()
print(df[[
    "year",
    "literary_creativity_score",
    "technical_density_score",
    "word_count",
    "personal_experience",
    "category",
    "title",
    "url",
]].head(30).to_string(index=False, max_colwidth=90))