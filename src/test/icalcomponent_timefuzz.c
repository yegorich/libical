/*======================================================================
 *
 * SPDX-FileCopyrightText: 2024 Contributors to the libical project <git@github.com:libical/libical>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Based on the original code from:
 * https://issues.oss-fuzz.com/issues/42538568
 *
 * ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <stdlib.h>

#include "libical/ical.h"

int main(int argc, char *argv[])
{
    FILE *fp;
    int fd, r;
    const char *fname;
#if defined(HAVE__FSTAT64)
    struct _stat64 sbuf;
#else
    struct stat sbuf;
#endif
    size_t filesize;
    void *data = NULL;
    icalcomponent *comp;

    if (argc != 2) {
        fname = TEST_DATADIR "/timefuzz71741";
    } else {
        fname = argv[1];
    }

    fp = fopen(fname, "rb");
    if (fp == (FILE *)NULL) {
        fprintf(stderr, "Error: unable to open %s\n", fname);
        assert(0);
    }

    fd = fileno(fp);
    if (fstat(fd, &sbuf) != 0) {
        fprintf(stderr, "Error: unable to fstat %s\n", fname);
        fclose(fp);
        assert(0);
    }
    filesize = (size_t)sbuf.st_size;
    data = malloc(filesize + 1);
    if (!data) {
        fprintf(stderr, "Error: unable to allocate memory\n");
        free(data);
        assert(0);
    }
    /* cppcheck-suppress nullPointerRedundantCheck */
    memset(data, 0, filesize + 1);

    r = read(fd, data, filesize);
    /* cppcheck-suppress doubleFree */
    fclose(fp);

    if (r < 0) {
        fprintf(stderr, "Error: Failed to read data\n");
        free(data);
        assert(0);
    }

    comp = icalcomponent_new_from_string(data);
    /* cppcheck-suppress doubleFree */
    free(data);

    if (comp != NULL) {
        (void)icalcomponent_get_span(comp);
    }
    icalcomponent_free(comp);

    return 0;
}
