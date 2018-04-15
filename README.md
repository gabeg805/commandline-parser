# Command Line Parser

## Introduction

The Command Line Parser is an easy to use command line parser for C++
programs. It takes 3 statements, and your program will be able to parse the
command line options, no problem. Because of the information provided by the
user, it is trivial to display a usage message; all the pertinent information is
already provided. If an error occurs, commandline::interface::usage() will be
called. For examples of how to use it, click [here](#example).

When declaring command line options, there are various different types of
options that can be used. The options are differentiated by what type of
arguments they may take:
- **no_argument**: This option does not take an argument.
- **optional_argument**: This option may or may not take an argument.
- **list_argument**: This argument takes one or more arguments. Arguments stop
    being read once a new option is encountered.
- **required_argument**: This option takes a single, required argument.

## Example

A standard way of initializing your command line options would be something like:
```
commandline::optlist_t options{
    {"-h",  "--help",           "",      commandline::no_argument,       "Print program usage."},
    {"-o",  "--option",         "title", commandline::optional_argument, "A command line option."},
    {"-a",  "--another-option", "title", commandline::list_argument,     "Another command line option."},
    {"-lo", "--longer-option",  "body",  commandline::required_argument, "A longer command line option."}
};
...
commandline::interface cli(options);
cli.parse(argv);
```

In order, this will do the following:
1. Define the possible command line options for your program.
2. Add them to the command line interface object so that it knows the
   possible options.
3. Parse the user input through the *argv* variable from main(argc, argv).

## Install

Copy the source and header files to the appropriate source and include
directories in your project.

**Important**: Make sure you have the following macro defined in the command
  line, or in the header file.
```
#define PROGRAM <program-name>
```

To add it in the command line, your *g++* options should look something like:
```
g++ ... -DPROGRAM="<program-name>..."
```

And if you're doing it in your Makefile, be sure to escape the quotes.
```
g++ ... -DPROGRAM="\"<program-name>...\""
```

## Uninstall

Remove the source and header files from your project.
