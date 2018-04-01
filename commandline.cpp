/**
 * @file commandline.cpp
 * @author Gabriel Gonzalez
 * 
 * @brief A command line interface utility to parse options, print usage, and
 *        notify the user when an error occurs.
 */

#include "commandline.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdio>

namespace commandline
{
    /**
     */
    interface::interface(const optlist_t options) : m_options(options)
    {
    }

    /**
     */
    void interface::usage(void)
    {
        printf("Usage: %s [option]...\n\n", PROGRAM);
        printf("Options:");
        char arg[commandline::kArgumentNameLength];

        for (option_t data : this->m_options)
        {
            if (data.name.empty())
            {
                arg[0] = '\0';
            }
            else
            {
                snprintf(arg, sizeof(arg), "=<%s>", data.name.c_str());
            }
            printf("\n    %s, %s%s\n", data.shortopt.c_str(),
                   data.longopt.c_str(), arg);
            printf("        %s\n", data.desc.c_str());
        }
    }

    /**
     * @details Iterate over the argument list. Check if the current arg has a
     *          list_argument type, and if it is, store each argument.
     *          Otherwise, the current arg either has a type which takes 0 or 1
     *          argument. Store the argument, if present, and increment the
     *          argument list vector by the proper amount.
     */
    void interface::parse(char** argv)
    {
        char**          argp     = argv+1;
        std::string     option   = *argp;
        bool            listflag = false;
        const option_t* data;

        for ( ; *argp != NULL; ++argp)
        {
            if (this->parse_list_argument(argp, option, listflag))
            {
                continue;
            }
            option = this->parse_option(&data, *argp);
            argp   = this->parse_argument(data, argp, listflag);
        }
    }

    /**
     * @details Used as a test to make sure that command line options were
     *          interpretted correctly. If there is ever any doubt, this
     *          function can be used.
     */
    void interface::test(void)
    {
        int i;
        for (auto it : this->m_table)
        {
            printf("%s: ", it.first.c_str());
            if (it.second.empty())
            {
                printf("\n");
                continue;
            }

            i = 0;
            for (auto a : it.second)
            {
                if (i > 0)
                {
                    printf(", ");
                }
                printf("%s", a.c_str());
                ++i;
            }
            printf("\n");
        }
    }

    /**
     */
    int interface::set(std::string option, std::string value)
    {
        std::string key = this->to_key(option);
        if (key.empty())
        {
            return -1;
        }
        this->m_table[key].push_back(value);
        return 0;
    }

    /**
     */
    std::string interface::get(std::string option)
    {
        return (this->has(option)) ?
            this->m_table[this->to_key(option)].at(0) : "";
    }

    /**
     */
    bool interface::has(std::string option)
    {
        return (this->m_table.find(this->to_key(option))
                != this->m_table.end());
    }

    /**
     */
    char* interface::parse_option(const option_t** data, char* option)
    {
        if (!(*data=this->find_option(option)))
        {
            fprintf(stderr, "%s: Invalid option '%s'\n", PROGRAM, option);
            exit(1);
        }
        return option;
    }

    /**
     */
    char** interface::parse_argument(const option_t* data, char** argp,
                                     bool& listflag)
    {
        std::string key = *argp;
        std::string value;
        if (!data)
        {
            return NULL;
        }

        switch (data->argument)
        {
        case commandline::no_argument:
            this->parse_help_option(data);
            return argp;
        case commandline::list_argument:
            listflag = true;
            if (!*(argp+1))
            {
                fprintf(stderr,
                        "%s: No argument after option '%s' with list_argument type.\n",
                        PROGRAM, *argp);
                exit(1);
            }
            return argp;
        case commandline::optional_argument:
        case commandline::required_argument:
        default:
            if (this->is_long_option(data, key))
            {
                argp = this->parse_long_argument(argp, key, value);
            }
            else if (this->is_short_option(data, key))
            {
                argp = this->parse_short_argument(argp, value);
            }
            else
            {
                fprintf(stderr,
                        "%s: Unable to determine if '%s' is a long or short option.\n",
                        PROGRAM, *argp);
                exit(1);
            }
            break;
        }

        this->set(key, value);
        return argp;
    }

    /**
     */
    void interface::parse_help_option(const option_t* data)
    {
        if ((data->longopt == "--help") || (data->shortopt == "-?"))
        {
            this->usage();
            exit(0);
        }
    }

    /**
     */
    char** interface::parse_short_argument(char** argp, std::string& value)
    {
        char* next = *(argp+1);
        if (next && !this->is_option(next))
        {
            value = next;
            ++argp;
        }
        else
        {
            value = "";
        }
        return argp;
    }

    /**
     */
    char** interface::parse_long_argument(char** argp, std::string& key,
                                          std::string& value)
    {
        value = this->extract_value(*argp);
        key   = this->extract_option(*argp);
        return argp;
    }

    /**
     * @details This function is meant to be called multiple times in different
     *          points of the argument list pointer, so as to capture all
     *          arguments of a list_argument type option.
     */
    bool interface::parse_list_argument(char** argp, std::string option, bool& listflag)
    {
        if (listflag)
        {
            if (this->is_option(*argp))
            {
                listflag = false;
            }
            else
            {
                this->set(option, *argp);
            }
        }
        return listflag;
    }

    /**
     */
    const option_t* interface::find_option(std::string option)
    {
        optlist_t::const_iterator it;
        const option_t* ptr;
        for (it=this->m_options.cbegin(); it != this->m_options.cend(); ++it)
        {
            ptr = &(*it);
            if (this->is_option(ptr, option))
            {
                return ptr;
            }
        }
        return NULL;
    }

    /**
     */
    std::string interface::extract(std::string option, int field)
    {
        if ((field != 1) && (field != 2))
        {
            return "";
        }
        if (option.find("=") == std::string::npos)
        {
            return (field == 1) ? option : "";
        }

        int length = option.length();
        int i;
        for (i=0; i < length; ++i)
        {
            if (option[i] == '=')
            {
                break;
            }
        }

        if (field == 1)
        {
            return option.substr(0, i);
        }
        ++i;
        return option.substr(i, length-i);
    }

    /**
     */
    std::string interface::extract_option(std::string option)
    {
        return this->extract(option, 1);
    }

    /**
     */
    std::string interface::extract_value(std::string option)
    {
        return this->extract(option, 2);
    }

    /**
     */
    std::string interface::to_short_option(std::string option)
    {
        const option_t* data = this->find_option(option);
        return (data) ? data->shortopt : "";
    }

    /**
     */
    std::string interface::to_long_option(std::string option)
    {
        const option_t* data = this->find_option(option);
        return (data) ? data->longopt : "";
    }

    /**
     * @details Check if the input string has any dashes in front. If not, try
     *          long option dashes first, and if that doesn't work, resort to
     *          the short option dash. Find the corresponding option struct,
     *          strip the leading dash(es) and return the key.
     * 
     *          The key will be used in m_table, and will have a corresponding
     *          value pair. By default, the long option is used as the key,
     *          without the leading dashes. However, if there is no long option,
     *          the short option is used, also without the leading dash.
     */
    std::string interface::to_key(std::string input)
    {
        const option_t* data = NULL;
        std::string     option;

        if (input[0] != '-')
        {
            input.insert(0, "--");
            if (!(data=this->find_option(input)))
            {
                input.erase(0, 1);
            }
        }

        if (!data && !(data=this->find_option(input)))
        {
            return "";
        }

        if (!data->longopt.empty())
        {
            option = data->longopt.substr(2);
        }
        else if (!data->shortopt.empty())
        {
            option = data->shortopt.substr(1);
        }
        else
        {
            return "";
        }

        return option;
    }

    /**
     */
    bool interface::is_option(std::string option)
    {
        return (this->is_short_option(option) || this->is_long_option(option));
    }

    /**
     */
    bool interface::is_option(const option_t* data, std::string option)
    {
        return (this->is_short_option(data, option)
                || this->is_long_option(data, option));
    }

    /**
     */
    bool interface::is_short_option(std::string option)
    {
        const option_t* data = this->find_option(option);
        return this->is_short_option(data, option);
    }

    /**
     */
    bool interface::is_short_option(const option_t* data, std::string option)
    {
        return (data && (option == data->shortopt));
    }

    /**
     */
    bool interface::is_long_option(std::string option)
    {
        const option_t* data = this->find_option(option);
        return this->is_long_option(data, option);
    }

    /**
     */
    bool interface::is_long_option(const option_t* data, std::string option)
    {
        return (data && ((option == data->longopt)
                || (this->extract_option(option) == data->longopt)));
    }
}
