![Static Analysis](https://github.com/nasa/tblCRCTool/workflows/Static%20Analysis/badge.svg)
![Format Check](https://github.com/nasa/tblCRCTool/workflows/Format%20Check/badge.svg)

# Core Flight System : Framework : Tool : Table CRC Generator

This repository contains NASA's Table CRC Generator Tool (tblCRCTool), which is a framework component of the Core Flight System.

This lab application is a ground utility to generate binary table CRCs for cFS. It is intended to be located in the `tools/tblCRCTool` subdirectory of a cFS Mission Tree.  The Core Flight System is bundled at https://github.com/nasa/cFS (which includes this tool as a submodule), which includes build and execution instructions.

## Version Notes

### Development Build: v1.3.0-rc4+dev7

- Remove explicit filename doxygen comments
- See <https://github.com/nasa/cFS/pull/432>

### Development Build: v1.3.0-rc4+dev2

- Set baseline for cFS-Caelum-rc4: v1.3.0-rc4
- Update mission rev to use 0xFF for development version
- See <https://github.com/nasa/cfs/pull/390>

### Development Build: v1.2.0-rc1+dev33

- Add printf conversion casts to fix Raspberry-Pi4 compilation error
  - Cast ssize_t to long and uint32 to unsigned long for printf.
  - This matches the %d and %lX conversions, respectively.
- Implement Coding Standard in CodeQL
- See <https://github.com/nasa/tblCRCTool/pull/52> and <https://github.com/nasa/cFS/pull/270>

### Development Build: v1.2.0-rc1+dev25

- Add Testing Tools to the Security Policy
- Check lseek return and exit/error processing updates
- See <https://github.com/nasa/tblCRCTool/pull/89>

### Development Build: v1.2.0-rc1+dev19

- Changes CLI "help" option to use two dashes: `--help`
- Adds static analysis and format check to continuous integration workflow. Adds workflow status badges to ReadMe.
- Adds CodeQL Analysis to continuous integration workflow.
- See <https://github.com/nasa/tblCRCTool/pull/35>

### Development Build: v1.2.0-rc1+dev12

- Documentation: Add `Security.md` with instructions on reporting vulnerabilities
- Removes unimplemented CRC cases to eliminate static analysis warnings
- See <https://github.com/nasa/tblCRCTool/pull/29>

### Development Build: v1.2.0-rc1+dev3

- Use `sizeof()` instead of a hard coded value for the table file header size to keep this tool in sync if the size of the cFE file or table header should ever change.
- Update version baseline to v1.2.0-rc1
- Set REVISION number to 99 to indicate development version
- See <https://github.com/nasa/tblCRCTool/pull/25>

### Development Build: 1.1.0+dev7

- Create a version header file
- Report version when responding to `-help` command
- See <https://github.com/nasa/tblCRCTool/pull/22>

### Development Build: 1.1.1

- Apply Code Style
- See <https://github.com/nasa/tblCRCTool/pull/18>

### **_OFFICIAL RELEASE: 1.1.0 - Aquila_**

- Minor updates (see https://github.com/nasa/tblCRCTool/pull/12)
- Released as part of cFE 6.7.0, Apache 2.0

### **_OFFICIAL RELEASE: 1.0.0a_**

- Released as part of cFE 6.6.0a, Apache 2.0

NOTE - there are other parameter set management schemes used with the cFS (JSON, csv, etc) which may be more applicable for modern missions.  Contact the community as detailed below for more information.

## Known issues

This ground utility was developed for a specific mission/configuration, and may not be applicable for general use.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at https://github.com/nasa/cFS.

Official cFS page: http://cfs.gsfc.nasa.gov
