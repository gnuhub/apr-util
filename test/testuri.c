/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000-2002 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "apr_general.h"
#include "apr_uri.h"

struct aup_test {
    const char *uri;
    apr_status_t rv;
    const char *scheme;
    const char *hostinfo;
    const char *user;
    const char *password;
    const char *hostname;
    const char *port_str;
    const char *path;
    const char *query;
    const char *fragment;
    apr_port_t  port;
};

struct aup_test aup_tests[] =
{
    {
        "http://127.0.0.1:9999/asdf.html",
        0, "http", "127.0.0.1:9999", NULL, NULL, "127.0.0.1", "9999", "/asdf.html", NULL, NULL, 9999
    },
    {
        "/error/include/top.html",
        0, NULL, NULL, NULL, NULL, NULL, NULL, "/error/include/top.html", NULL, NULL, 0
    },
    {
        "/error/include/../contact.html.var",
        0, NULL, NULL, NULL, NULL, NULL, NULL, "/error/include/../contact.html.var", NULL, NULL, 0
    },
    {
        "/",
        0, NULL, NULL, NULL, NULL, NULL, NULL, "/", NULL, NULL, 0
    },
    {
        "/manual/",
        0, NULL, NULL, NULL, NULL, NULL, NULL, "/manual/", NULL, NULL, 0
    },
    {
        "/cocoon/developing/graphics/Using%20Databases-label_over.jpg",
        0, NULL, NULL, NULL, NULL, NULL, NULL, "/cocoon/developing/graphics/Using%20Databases-label_over.jpg", NULL, NULL, 0
    },
    {
        "http://sonyamt:garbage@127.0.0.1/filespace/",
        0, "http", "sonyamt:garbage@127.0.0.1", "sonyamt", "garbage", "127.0.0.1", NULL, "/filespace/", NULL, NULL, 0
    },
    {
        "http://sonyamt@127.0.0.1/filespace/?arg1=store",
        0, "http", "sonyamt@127.0.0.1", "sonyamt", NULL, "127.0.0.1", NULL, "/filespace/", "arg1=store", NULL, 0
    }
};

static void show_info(apr_status_t rv, apr_status_t expected, const apr_uri_t *info)
{
    if (rv != expected) {
        fprintf(stderr, "  actual rv: %d    expected rv:  %d\n", rv, expected);
    }
    else {
        fprintf(stderr, 
                "  scheme:           %s\n"
                "  hostinfo:         %s\n"
                "  user:             %s\n"
                "  password:         %s\n"
                "  hostname:         %s\n"
                "  port_str:         %s\n"
                "  path:             %s\n"
                "  query:            %s\n"
                "  fragment:         %s\n"
                "  hostent:          %p\n"
                "  port:             %u\n"
                "  is_initialized:   %u\n"
                "  dns_looked_up:    %u\n"
                "  dns_resolved:     %u\n",
                info->scheme, info->hostinfo, info->user, info->password,
                info->hostname, info->port_str, info->path, info->query,
                info->fragment, info->hostent, info->port, info->is_initialized,
                info->dns_looked_up, info->dns_resolved);
    }
}

static int same_str(const char *s1, const char *s2)
{
    if (s1 == s2) { /* e.g., NULL and NULL */
        return 1;
    }
    else if (!s1 || !s2) { /* only 1 is NULL */
        return 0;
    }
    else {
        return strcmp(s1, s2) == 0;
    }
}

static int test_aup(apr_pool_t *p)
{
    int i;
    apr_status_t rv;
    apr_uri_t info;
    struct aup_test *t;
    const char *failed;
    int rc = 0;

    for (i = 0; i < sizeof(aup_tests) / sizeof(aup_tests[0]); i++) {
        memset(&info, 0, sizeof(info));
        t = &aup_tests[i];
        rv = apr_uri_parse(p, t->uri, &info);
        failed = (rv != t->rv) ? "bad rc" : NULL;
        if (!failed) {
            if (!same_str(info.scheme, t->scheme))
                failed = "bad scheme";
            if (!same_str(info.hostinfo, t->hostinfo))
                failed = "bad hostinfo";
            if (!same_str(info.user, t->user))
                failed = "bad user";
            if (!same_str(info.password, t->password))
                failed = "bad password";
            if (!same_str(info.hostname, t->hostname))
                failed = "bad hostname";
            if (!same_str(info.port_str, t->port_str))
                failed = "bad port_str";
            if (!same_str(info.path, t->path))
                failed = "bad path";
            if (!same_str(info.query, t->query))
                failed = "bad query";
            if (!same_str(info.fragment, t->fragment))
                failed = "bad fragment";
            if (info.port != t->port)
                failed = "bad port";
        }
        if (failed) {
            ++rc;
            fprintf(stderr, "failure for testcase %d/uri %s: %s\n", i,
                    t->uri, failed);
            show_info(rv, t->rv, &info);
        }
    }

    return rc;
}

int main(void)
{
    apr_pool_t *pool;
    int rc;

    apr_initialize();
    atexit(apr_terminate);

    apr_pool_create(&pool, NULL);

    rc = test_aup(pool);

    return rc;
}