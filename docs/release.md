# Release Process

This project uses GitHub Actions to produce release artifacts.

Release workflow file:

- [.github/workflows/release.yml](../.github/workflows/release.yml)

Nightly validation workflow file:

- [.github/workflows/nightly.yml](../.github/workflows/nightly.yml)

## Automatic Release (recommended)

Create and push a semantic version tag:

```bash
git checkout main
git pull --ff-only origin main
git tag vX.Y.Z
git push origin vX.Y.Z
```

When the tag is pushed, the workflow:

1. builds Linux binaries
2. builds Windows UCRT64 binaries
3. builds Doxygen HTML docs
4. creates a GitHub Release with generated notes
5. uploads all packaged artifacts to the release

## Manual Release

You can run the `Release` workflow manually from GitHub Actions (`workflow_dispatch`) and provide:

- `tag`: release tag to publish (example `v1.2.3`)

## Published Artifacts

- `lbr-linux-x64.tar.gz`
- `lbr-windows-ucrt64-x64.zip`
- `lbr-docs-html.tar.gz`

## Notes

- Use semantic tags (`vMAJOR.MINOR.PATCH`) for predictable automation.
- Release binaries are built in `Release` mode.
- Docs archive contains the generated Doxygen HTML tree.
- Nightly runs are intended for broader regression coverage and early drift detection.