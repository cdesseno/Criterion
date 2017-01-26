/*
 * The MIT License (MIT)
 *
 * Copyright © 2015-2016 Franklin "Snaipe" Mathieu <http://snai.pe/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "criterion/stats.h"
#include "criterion/options.h"
#include "criterion/internal/ordered-set.h"
#include "log/logging.h"
#include "compat/posix.h"
#include "compat/strtok.h"
#include "compat/time.h"
#include "config.h"
#include "common.h"

/*
 * {"tests": [
 * 			{"name": "test_1", "success": true, "description": "passed"}, 
 * 			{"name": "test_2", "success": false, "description": "failure"}
 * 			],
 * "success": false
 * }
 * */
 
static CR_INLINE const char *get_status_string(struct criterion_test_stats *ts)
{
    return (ts->crashed || ts->timed_out)           ? "error"
           : ts->test_status == CR_STATUS_FAILED     ? "failure"
           : ts->test_status == CR_STATUS_SKIPPED    ? "skip"
           : "passed";
}

int getPassed(struct criterion_test_stats *ts) {
    return (ts->crashed || ts->timed_out)           ? 1
           : ts->test_status == CR_STATUS_FAILED     ? 2
           : ts->test_status == CR_STATUS_SKIPPED    ? 3
           : 0;
}

// Based on src/io/json.c
void custom_json_report(FILE *f, struct criterion_global_stats *stats){
           
    fprintf(f, "{");
    fprintf(f, "\"tests\": [");
    
    int success = 0;
    for (struct criterion_suite_stats *ss = stats->suites; ss; ss = ss->next) {

        for (struct criterion_test_stats *ts = ss->tests; ts; ts = ts->next) {
            
            fprintf(f, "{\"name\": \"%s\", \"success\": %s, \"description\": \"%s\"}",
				ts->test->name,  (getPassed(ts) == 0) ? "true" : "false",
				get_status_string(ts)
            );
            
            fprintf(f, ts->next ? "," : "");
            success = success + getPassed(ts);
        }
        //fprintf(f, ss->next ? ",\n" : "\n");
    }
    
    fprintf(f, "],");
    
    if (success > 0) {
		fprintf(f, "\"success\": false");
	} else {
		fprintf(f, "\"success\": true");
	}
    
    fprintf(f, "}");
}
