

# Running MarlinMT with Helgrind

Helgrind is a tool available in Valgrind to detect multi-threading problems such as data races, race conditions, etc...

Unfortunately, Helgrind sometimes makes some false-positive, such as data-races on atomic variables. For example:

- use of an atomic variable in different threads will produce a error message
- the destructor of shared_ptr will produce an error message, even if the internal counter is protected via an atomic operation

In this directory, we provide a suppression file, namely `helgrind-suppression.supp`. This file contains false-positives coming from Marlin/MarlinMT to be ignored while running Helgrind.

To run MarlinMT with Helgrind use the following command line:

```shell
valgrind --tool=helgrind --suppressions=$MARLIN/valgrind/helgrind-suppression.supp MarlinMT your_steering_file.xml
```

This should suppress the known false positives coming from Marlin. If you notice a few more popping up while running with Helgind, they can be of different types:

- false-positives we haven't detected from MarlinMT. In this case, please open an issue in the iLCSoft/Marlin Github repository.
- true errors from MarlinMT. Then same as before, report it.
- any kind or error coming from your processors, so not from the Marlin framework. In this case we are not responsible, but you should investigate your issues and fix them if necessary.

In case you need to post some error in a Github issue, please re-run the command with `--gen-suppressions=all`. 

```shell
valgrind --tool=helgrind --gen-suppressions=all --suppressions=$MARLIN/valgrind/helgrind-suppression.supp MarlinMT your_steering_file.xml
```

This will print a suppression after every error found, and you are kindly asked to post it along with the Github issue.
