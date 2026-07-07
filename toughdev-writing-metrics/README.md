# ToughDev Writing Metrics

A small reproducible analysis project for studying how the writing style of [ToughDev](https://www.toughdev.com) changed from 2008 to 2026.

The project crawls public ToughDev article pages, extracts article and paragraph text, calculates deterministic writing metrics, and uses a local LLM to classify first-person paragraphs into rough personal-experience categories.

This repository accompanies the article:

**What the Metrics Remember: Almost Two Decades of Technical Writing on ToughDev**

The main question is simple:

> Did ToughDev change from short technical notes into something closer to technical memoir?

The answer is not decided by the script. The script only points to paragraphs and years worth reading more carefully.

## Summary of the dataset

The current run produced:

```text
Articles scanned: 357
Year range: 2008–2026
Total words scanned: 323,028
Candidate first-person paragraphs: 2,165
AI-classified personal-experience paragraphs: 543
Parse errors: 0