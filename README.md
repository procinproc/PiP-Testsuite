# PiP Testsuite

Testsuite of Process-in-Process

This is a testsuite of Process-in-Process ([PiP]
(https://github.com/RIKEN-SysSoft/PiP)) covering all PiP API
versions.  This package can also be installed by the PiP package
installing program
([PiP-pip](https://github.com/RIKEN-SysSoft/PiP-pip)) and once the
installation is finished the installed PiP will be automatically
tested by using this program.

# License

This package is licensed under the 2-clause simplified BSD License -
see the [LICENSE](LICENSE) file for details.

# Usage

    $ cd TOP_SRCDIR
    $ ./configure --with-pip=<PIP_INSTALLED_DIR>
    $ make test

# Author

Atsushi Hori<br>
Riken Center for Commputational Science (R-CCS)<br>
Japan<br>
