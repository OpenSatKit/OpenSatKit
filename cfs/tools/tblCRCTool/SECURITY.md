# Security Policy

## Reporting a Vulnerability

To report a vulnerability for the tblCRCTool subsystem please [submit an issue](https://github.com/nasa/tblCRCTool/issues/new/choose).

For general cFS vulnerabilities please [open a cFS framework issue](https://github.com/nasa/cfs/issues/new/choose) and see our [top-level security policy](https://github.com/nasa/cFS/security/policy) for additional information.

In either case please use the "Bug Report" template and provide as much information as possible. Apply appropriate labels for each report. For security related reports, tag the issue with the "security" label.

## Testing

**Disclaimer: nasa/tblCRCTool is not responsible for any liability incurred under the [Apache License 2.0](https://github.com/nasa/tblCRCTool/blob/main/LICENSE).**

Testing is an important aspect our team values to improve tblCRCTool. 

To view tools used for the cFS bundle, see our [top-level security policy](https://github.com/nasa/cFS/security/policy). 

### CodeQL

The [tblCRCTool CodeQL GitHub Actions workflow](https://github.com/nasa/tblCRCTool/actions/workflows/codeql-build.yml) is available to the public. To review the results, fork the tblCRCTool repository and run the CodeQL workflow. 

CodeQL is ran for every push and pull-request on all branches of tblCRCTool in GitHub Actions. 

For the CodeQL GitHub Actions setup, visit https://github.com/github/codeql-action. 

### Cppcheck

The [tblCRCTool Cppcheck GitHub Actions workflow and results](https://github.com/nasa/tblCRCTool/actions/workflows/static-analysis.yml) are available to the public. To view the results, select a workflow and download the artifacts. 

Cppcheck is ran for every push on the main branch and every pull request on all branches of tblCRCTool in Github Actions. 

For more information about Cppcheck, visit http://cppcheck.sourceforge.net/.

## Additional Support

For additional support, submit a GitHub issue. You can also email the cfs community at cfs-community@lists.nasa.gov. 

You can subscribe to the mailing list [here](https://lists.nasa.gov/mailman/listinfo/cfs-community) that includes all the community members/users of the NASA core Flight Software (cFS) product line. The mailing list is used to communicate any information related to the cFS product such as current releases, bug findings and fixes, enhancement requests, community meeting notifications, sending out meeting minutes, etc.

If you wish to report a cybersecurity incident or concern, please contact the NASA Security Operations Center either by phone at 1-877-627-2732 or via email address soc@nasa.gov.
