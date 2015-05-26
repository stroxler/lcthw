echo "Running unit tests"

# create a header for this run in the log
echo "\n" >> tests/tests.log
echo "Starting Test Suite "$(date) >> tests/tests.log
echo "================================================" >> tests/tests.log

for tf in tests/*_tests; do


    if test -f $tf; then
        # Create a header for this file in the log

        # run the test, adding to log, and if exit code is non-zero
        # give an error message & abort.
        if $VALGRIND ./${tf} 2 >> tests/tests.log; then
            echo "$tf PASS"
        else
            echo "ERROR in test $tf! Here's tests/test.log:"
            tail tests/tests.log
            exit 1  # I'm not sure I agree with this... sometimes it's useful
                    # to see a full set of which tests failed in a regression.
        fi
    fi
done

echo ""

