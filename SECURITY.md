# Security and data handling

This preview executes a user-supplied native game program. The supported-build/hash checks catch mistakes and corruption; they do not make arbitrary native code trustworthy. Import only files generated from your own known copy. The importer confines extraction to staging, limits size/count, verifies manifest entries and replaces resources only after validation. The manifest is integrity metadata, not a publisher signature.

Do not put keys, game files, signing secrets or saves in public reports. For a security issue, use the hosting provider's private vulnerability reporting if enabled; otherwise ask the maintainer for a private channel without publishing exploit details or secrets. This local repository does not configure a security inbox.

The shell is a development-signed, debuggable sideload preview, not a hardened store application. App-private files and saves are removed on uninstall. Keep personal backups and signing keys private.
