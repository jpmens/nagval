# NAME

nagval - Nagios/Icinga plugin to check validity of one or more
DNSSEC domains

# SYNOPSIS

nagval [-f *file*] [*domain* *rr*]

# DESCRIPTION

*nagval* (i.e. "Nagios Validator") is a Nagios/Icinga plugin to
check the validity of a DNSSEC-signed zone over the DNS.

Typical use of *nagval* as a Nagios plugin is to specify a single
*domain* and *resource record type*. Optionally, a list of domains
can be provided in a file.

# OPTIONS

*nagval* understands the following options.

-f *file*
:   Specify a file containing a list of domains to check. The file
    must contain a list of domains, one per line, where each domain is
    optionally followed by a slash (/) and a DNS resource record type
    to check. The type defaults to *SOA* if it is not specified.

Example:

    $ cat domains
    google.com/A
    orange.kame.net/AAAA
    ip.jpmens.org/a
    infoblox.com/NS
    jasadvisors.com/DNSKEY
    verisignlabs.com
    chainzombies.com/A
    sanibar.com
    ibadancer.com
    wnagele.com/A
    b.aa/soa

# BUGS

Yes.

# RETURN CODES

*nagval* exits with a code 0, 1, or 2 indicating a status of OK,
WARNING, or CRITICAL. Currently, if a domain does not validate a
status CRITICAL is issued, else OK.

# AVAILABILITY

<http://github.com/jpmens/nagval>

# CREDITS

-   This program requires *dnsval*, a library provided by the
    DNSSEC-tools project <https://www.dnssec-tools.org/>

# SEE ALSO

-   `resolver`(5).
-   <http://tools.ietf.org/html/draft-hayatnagarkar-dnsext-validator-api-07>

# AUTHOR

Jan-Piet Mens <http://mens.de>



