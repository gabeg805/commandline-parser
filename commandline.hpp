/**
 * @file commandline.hpp
 * @author Gabriel Gonzalez
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
 */

#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @namespace commandline
 * 
 * @brief Command line interface utility.
 */
namespace commandline
{
    /**
     * @brief Maximum length of an argument name, when printing out program
     *         usage.
     */
    const size_t kArgumentNameLength = 32;

    /**
     * @enum argument_t
     * 
     * @brief The type of argument for an option.
     */
    enum argument_t
    {
        no_argument,       /**< No argument. */
        required_argument, /**< One required argument after an option. */
        optional_argument, /**< An optional argument after an option. */
        list_argument      /**< One or more arguments after an option. */
    };

    /**
     * @struct option
     * 
     * @brief A structure containing all the information one needs to know about
     *        an option.
     * 
     * @details All the information about an option, such as: the short form,
     *          long form, argument name, argument type, and a description of
     *          the option.
     */
    struct option
    {
        std::string shortopt; /**< Short form of the option. */
        std::string longopt;  /**< Long form of the option. */
        std::string name;     /**< Name of the argument. */
        argument_t  argument; /**< Type of argument. */
        std::string desc;     /**< Description of the option. */
    };

    /**
     * @brief Type name for an option.
     */
    typedef struct option option_t;

    /**
     * @brief Type name for a list of all options in a program.
     */
    typedef std::vector<option_t> optlist_t;

    /**
     * @brief Type name for list of all entered options and their respective
     *        values.
     */
    typedef std::unordered_map<std::string, std::vector<std::string>> keyval_t;

    /**
     * @class interface
     * 
     * @brief Interface to the command line, where a user will parse command
     *        line options, print program usage, etc.
     */
    class interface
    {
    public:
        /**
         * @brief Construct the command line interface.
         * 
         * @param[in] options List of all command line options for the program.
         */
        explicit interface(const optlist_t options);

        /**
         * @brief Print the program usage message.
         * 
         * @note PROGRAM needs to be defined as a macro.
         * @note kArgumentNameLength represents the longest name an argument can
         *       have. Resize this in the header file if it needs to be larger.
         */
        void usage(void);

        /**
         * @brief Parse the list of arguments given on the command line.
         * 
         * @param[in] argv List of command line arguments, typically from the
         *                 main(argc, argv) function.
         */
        void parse(char** argv);

        /**
         * @brief Print the command line options that have been entered, to
         *        ensure they were read correctly.
         */
        void test(void);

        /**
         * @brief Set the value for the given option.
         * 
         * @param[in] option An option entered in the command line.
         * @param[in] value  The value to set for the given option.
         * 
         * @return If successful, return 0. When unable to find a key for the
         *         option, return -1.
         */
        int set(std::string option, std::string value);

        /**
         * @brief Retrieve the value for the given option.
         * 
         * @param[in] option An option entered in the command line.
         * 
         * @return If successful, return the value connected pertaining to the
         *         given option. If unable to find the key for the given option,
         *         return an empty string.
         */
        std::string get(std::string option);

        /**
         * @brief Check if the given option has been entered on the command
         *        line.
         * 
         * @param[in] option An option entered in the command line.
         * 
         * @return true if the option is found in the hash table, and false if
         *         it is unable to be found.
         */
        bool has(std::string option);

    private:
        /**
         * @brief List of all possible options that can be supplied to the
         *        program.
         */
        const optlist_t m_options;

        /**
         * @brief A perfect hash table containing the options that were supplied
         *        in the command line, and their corresponding values.

         * @details The keys for this hash table are the option strings that are
         *          supplied, e.g. '--long-option' or '-short', but without the
         *          leading dash(es). Long options are used as the default key,
         *          but if no long option exists, it falls back to the short
         *          option.
         */
        keyval_t m_table;

        /**
         * @brief Determine if the input option is in fact a valid option.
         * 
         * @param[out] data   Command line option struct.
         * @param[in]  option Command line option string.
         * 
         * @return The option string if a valid option. Exit program otherwise.
         */
        char* parse_option(const option_t** data, char* option);

        /**
         * @brief Determine the argument type and store the corresponding value,
         *        if the argument type takes a value.
         * 
         * @param[in]  data     Data structure for an option.
         * @param[in]  argp     Argument list pointer, pointing to the
         *                      current command line option.
         * @param[out] listflag Used to set the list flag if a list argument
         *                      is found.
         * 
         * @return The pointer to the current argument in the argument list.
         *         NULL if an error occurs.
         * 
         * @note If the long option '--help' is found, usage() will be called.
         */
        char** parse_argument(const option_t* data, char** argp,
                              bool& listflag);

        /**
         * @brief Check if the command line option struct is for the --help
         *        option. Print usage and exit successfully if it is.
         * 
         * @param[in] data Command line option struct.
         */
        void parse_help_option(const option_t* data);

        /**
         * @brief For the given short option, determine its corresponding
         *        argument, if there is one.
         * 
         * @param[in,out] argp  The argument list pointer. This will be
         *                      incremented if there are more command line
         *                      options after the current one.
         * @param[out]    value The argument to the current option. If there is
         *                      no argument, this will be an empty string.
         * 
         * @return The argument list pointer, properly incremented if there was
         *         an argument following the current option.
         */
        char** parse_short_argument(char** argp, std::string& value);

        /**
         * @brief For the given short option, determine its corresponding
         *        argument, if there is one.
         * 
         * @param[in,out] argp  The argument list pointer. This will be
         *                      incremented if there are more command line
         *                      options after the current one.
         * @param[out]    key   The current option string. For long options,
         *                      this must be extracted from the full string
         *                      '--long-option=value'.
         * @param[out]    value The argument to the current option. If there is
         *                      no argument, this will be an empty string.
         * 
         * @return The argument list pointer. It is not modified throughout the
         *         duration of the function.
         */
        char** parse_long_argument(char** argp, std::string& key,
                                   std::string& value);

        /**
         * @brief Check if there is a list argument, and if there is, store the
         *        argument(s).
         * 
         * @param[in]     argp     Argument list pointer, pointing to the
         *                         current argument.
         * @param[in]     option   The key used to set a value in m_table.
         * @param[in,out] listflag A flag indicating if the previously found
         *                         option contains list type arguments.
         * 
         * @return true if the previously found option has a list argument type.
         *         Otherwise, return false.
         */
        bool parse_list_argument(char** argp, std::string option,
                                 bool& listflag);

        /**
         * @brief Find an option struct that has an option string that matches
         *        the input string.
         * 
         * @param[in] option The option string to search for.
         * 
         * @return The option struct found, that contains the option
         *         string. Otherwise, return NULL.
         */
        const option_t* find_option(std::string option);

        /**
         * @brief Extract either the option or value from a long option string.
         *        
         * @param[in] option The long option string of the form
         *                   '--long-option=value'.
         * @param[in] field  A field of 1 means the '--long-option' section and a
         *                   field of 2 means the 'value' section.
         * 
         * @return The substring requested by the user. If field is an improper
         *         value, return an empty string. If the length of the string is
         *         iterated over and no '=' is found, return the given option
         *         string.
         */
        std::string extract(std::string option, int field);

        /**
         * @brief Extract the long option section from a long option string.
         * 
         * @param[in] option The long option string.
         * 
         * @return See extract().
         */
        std::string extract_option(std::string option);

        /**
         * @brief Extract the value section from a long option string.
         * 
         * @param[in] option The long option string.
         * 
         * @return See extract().
         */
        std::string extract_value(std::string option);

        /**
         * @brief Convert an option string, long or short, to a short option.
         * 
         * @param[in] option An option string.
         * 
         * @return The short option if it is found. Otherwise, return an empty
         *         string.
         */
        std::string to_short_option(std::string option);

        /**
         * @brief Convert an option string, long or short, to a long option.
         * 
         * @param[in] option An option string.
         * 
         * @return The long option if it is found. Otherwise, return an empty
         *         string.
         */
        std::string to_long_option(std::string option);

        /**
         * @brief Convert input option to a key string. This means
         *        '--long-option' is converted to 'long-option' and if there is
         *        no long option, then '-short' is converted to 'short'.
         * 
         * @param[in] input The option string to convert a key.
         * 
         * @return The key. Otherwise, return an empty string.
         */
        std::string to_key(std::string input);

        /**
         * @brief Check if the given option is a valid short or long command
         *        line option.
         * 
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_option(std::string option);

        /**
         * @brief Check if the given option is a valid short or long command
         *        line option.
         * 
         * @param[in] data   An option struct.
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_option(const option_t* data, std::string option);

        /**
         * @brief Check if the given option is a valid short command line
         *        option.
         * 
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_short_option(std::string option);

        /**
         * @brief Check if the given option is a valid short command line
         *        option.
         * 
         * @param[in] data   An option struct.
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_short_option(const option_t* data, std::string option);

        /**
         * @brief Check if the given option is a valid long command line option.
         * 
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_long_option(std::string option);

        /**
         * @brief Check if the given option is a valid long command line option.
         * 
         * @param[in] data   An option struct.
         * @param[in] option An option string.
         * 
         * @return true if the input is an option, and false otherwise.
         */
        bool is_long_option(const option_t* data, std::string option);
    };

}

#endif /* COMMAND_LINE_HPP */
