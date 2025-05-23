/*======================================================================
 FILE: recur.c
 CREATOR: ebusboom 8jun00

 SPDX-FileCopyrightText: 1999 Eric Busboom <eric@civicknowledge.com>

 DESCRIPTION:

 Test program for expanding recurrences. Run as:

     ./recur ../../test-data/recur.txt

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"
#include "libicalss/icalss.h"

#include <stdlib.h>

#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-unsafe-call-within-signal-handler"
#endif
static void sig_alrm(int i)
{
    _unused(i);
    fprintf(stderr, "Could not get lock on file\n");
    exit(1);
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#endif

static void recur_callback(icalcomponent *comp, struct icaltime_span *span, void *data)
{
    _unused(comp);
    _unused(data);
    printf("cb: %s", icalctime(&span->start));
    printf("    %s\n", icalctime(&span->end));
}

int main(int argc, char *argv[])
{
    icalset *cin;
    struct icaltimetype next;
    icalcomponent *itr;
    icalproperty *desc, *dtstart, *rrule;
    struct icalrecurrencetype *recur;
    icalrecur_iterator *ritr;
    icaltime_t tt;
    const char *file;

    icalerror_set_error_state(ICAL_PARSE_ERROR, ICAL_ERROR_NONFATAL);

#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
    (void)signal(SIGALRM, sig_alrm);
#endif

    if (argc <= 1) {
        file = TEST_DATADIR "/recur.txt";
    } else if (argc == 2) {
        file = argv[1];
    } else {
        fprintf(stderr, "usage: recur [input file]\n");
        exit(1);
    }

#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
    alarm(300); /* to get file lock */
#endif
    cin = icalfileset_new(file);
#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
    alarm(0);
#endif

    if (cin == 0) {
        fprintf(stderr, "recur: can't open file %s\n", file);
        exit(1);
    }

    for (itr = icalfileset_get_first_component(cin);
         itr != 0; itr = icalfileset_get_next_component(cin)) {
        struct icaltimetype start;
        struct icaltimetype end = icaltime_today();

        desc = icalcomponent_get_first_property(itr, ICAL_DESCRIPTION_PROPERTY);
        dtstart = icalcomponent_get_first_property(itr, ICAL_DTSTART_PROPERTY);
        rrule = icalcomponent_get_first_property(itr, ICAL_RRULE_PROPERTY);

        if (desc == 0 || dtstart == 0 || rrule == 0) {
            printf("\n******** Error in input component ********\n");
            printf("The following component is malformed:\n %s\n",
                   icalcomponent_as_ical_string(itr));
            continue;
        }

        printf("\n\n#### %s\n", icalproperty_get_description(desc));
        printf("#### %s\n", icalvalue_as_ical_string(icalproperty_get_value(rrule)));
        recur = icalproperty_get_rrule(rrule);
        start = icalproperty_get_dtstart(dtstart);

        ritr = icalrecur_iterator_new(recur, start);

        tt = icaltime_as_timet(start);

        printf("#### %s\n", icalctime(&tt));

        icalrecur_iterator_free(ritr);

        ritr = icalrecur_iterator_new(recur, start);
        for (next = icalrecur_iterator_next(ritr);
             !icaltime_is_null_time(next);
             next = icalrecur_iterator_next(ritr)) {
            tt = icaltime_as_timet(next);
            printf("  %s", icalctime(&tt));
        }
        icalrecur_iterator_free(ritr);

        icalcomponent_foreach_recurrence(itr, start, end, recur_callback, NULL);
    }

    icalset_free(cin);

    icaltimezone_free_builtin_timezones();

    icalmemory_free_ring();

    free_zone_directory();

    return 0;
}
