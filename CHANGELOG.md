# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
- Using a custom variable in InstallBasicPackageFile to backup the module path: [#47](https://github.com/ami-iit/matio-cpp/pull/47).
- Added more checks before creating a file: [#49](https://github.com/ami-iit/matio-cpp/pull/49).

## [0.1.1] - 2021-03-22

- Do not use find_dependency in matioCppConfig.cmake if OVERRIDE_MODULE_PATH is used: [#41](https://github.com/ami-iit/matio-cpp/pull/41).
- Make sure to add a NULL terminator in the cases where the new matio version uses strlen: [#44](https://github.com/ami-iit/matio-cpp/pull/44).
- Allow the assignment from string also for non standard strings: [#45](https://github.com/ami-iit/matio-cpp/pull/45).

## [0.1.0] - 2021-02-02

- First version implemented.
