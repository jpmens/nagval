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

# MOTIVATION

Why another DNSSEC validator? There are lots of those around
already! Yes, there are (see below). Most of them retrieve an RRSIG
and perform date arithmetic on the expiration times. Based on an
idea I read in a paper by Stéphane Bortzmeyer, I decided to have a
validating resolving DNS server do the brunt of the validation work
-- after all, that's what it's supposed to be good at, right?

That is what *nagval* does. It used the *dnsval* library from the
DNSSEC-Tools project to send a query to a validating resolver,
evaluating the results returned in the answer. For example, If I
check a domain using a BIND validating resolver

    $ nagval jpmens.org SOA
    jpmens.org/SOA: SUCCESS

I see the following in BIND's log

    query: jpmens.org IN SOA +EDC (127.0.0.1)
    query: jpmens.org IN DNSKEY +EDC (127.0.0.1)
    query: jpmens.org IN DS +EDC (127.0.0.1)
    query: org IN DNSKEY +EDC (127.0.0.1)
    query: org IN DS +EDC (127.0.0.1)
    query: . IN DNSKEY +EDC (127.0.0.1)

Note how *nagval* issued queries with EDNS0 DNSSEC OK (DO) and
Checking Disabled (CD).

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
-   <https://github.com/dotse/dnssec-monitor>
-   <http://dns.measurement-factory.com/tools/nagios-plugins/check_zone_rrsig_expiration.html>
-   <http://zonecheck.fr>

# AUTHOR

Jan-Piet Mens <http://mens.de>



