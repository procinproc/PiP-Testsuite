# PiP Testsuite

Testsuite of Process-in-Process

This is a testsuite of Process-in-Process ([PiP]
(https://github.com/procinproc/PiP)) covering all PiP API
versions.  This package can also be installed by the PiP package
installing program
([PiP-pip](https://github.com/procinproc/PiP-pip)) and once the
installation is finished the installed PiP will be automatically
tested.

# License

This package is licensed under the 2-clause simplified BSD License -
see the [LICENSE](LICENSE) file for details.

# Usage

    $ cd TOP_SRCDIR
    $ ./configure --with-pip=<PIP_INSTALLED_DIR>
    $ make test

Here is the sample output of the test results;

    ...
    NTASKS:   300
    NTHREADS: 32
    testing L - process:preload
    testing G - process:got
    testing C - process:pipclone
    testing T - pthread
    
    >>> utils/test-v2.list
    >>> utils.list
    utils.list: NG timer 0 inf                                                         L -- PASS
    utils.list: NG timer 0 inf                                                         G -- PASS
    ...
    prog-v2.list: timer 1 OK pip-exec -n 4 ./xyz 0 0 1 2 3 : -n 3 ./xyz 4 4 5 6        C -- PASS
    prog-v2.list: timer 1 OK pip-exec -n 4 ./xyz 0 0 1 2 3 : -n 3 ./xyz 4 4 5 6        T -- PASS
    <<< prog-v2.list
    <<< prog/test-v2.list
    
    Total test: 596
      success            : 574
      failure            : 0
      unsupported        : 22

# Mailing Lists

If you have any questions or comments on PiP-Testsuite and/or PiP iteself, 
send e-mails to;

    procinproc-info@googlegroups.com

Or, join our PiP users' mailing list;

    procinproc-users@googlegroups.com

# Author

Atsushi Hori<br>
Riken Center for Commputational Science (R-CCS)<br>
Japan<br>
