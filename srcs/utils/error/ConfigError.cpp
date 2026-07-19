#include "utils/error/ConfigError.hpp"

void throwConfigError(ConfigErrorType type, std::string const& detail)
{
    std::string msg;
    switch (type)
    {
        case ERR_CFG_CANNOT_OPEN:        msg = "Cannot open config file: " + detail;                    break;
        case ERR_CFG_MISSING_SERVER:     msg = "Missing server block in: " + detail;                     break;
        case ERR_CFG_MISSING_OPEN_BRACE: msg = "Missing '{' after '" + detail + "'";                     break;
        case ERR_CFG_UNCLOSED_SERVER:    msg = "Unclosed server block (missing '}')";                break;
        case ERR_CFG_UNCLOSED_LOCATION:  msg = "Unclosed location block '" + detail + "' (missing '}')"; break;
        case ERR_CFG_MISSING_SEMICOLON:  msg = "Missing ';' after '" + detail + "' value";              break;
        case ERR_CFG_EMPTY_VALUE:        msg = "Empty value for '" + detail + "'";                      break;
        case ERR_CFG_INVALID_PORT:       msg = "Invalid port value: '" + detail + "'";                  break;
        case ERR_CFG_PORT_RANGE:         msg = "Port out of range: " + detail;                          break;
        case ERR_CFG_INVALID_CODE:       msg = "Invalid error_page code: '" + detail + "'";             break;
        case ERR_CFG_MISSING_ERROR_PAGE: msg = "Missing page path for error_page " + detail;             break;
        case ERR_CFG_EMPTY_METHODS:      msg = "Empty 'methods' directive";                          break;
        case ERR_CFG_MISSING_PATH:       msg = "Missing path after 'location'";                          break;
        case ERR_CFG_MISSING_DIRECTIVE:  msg = "Missing '" + detail + "' directive in server block";    break;
        case ERR_CFG_DUPLICATE_DIRECTIVE: msg = "Duplicate '" + detail + "' directive";                 break;
        case ERR_CFG_UNKNOWN_DIRECTIVE:  msg = "Unknown directive: '" + detail + "'";                    break;
        case ERR_CFG_INVALID_METHOD:     msg = "Invalid HTTP method: '" + detail + "'";                  break;
        case ERR_CFG_CODE_RANGE:         msg = "error_page code out of range: " + detail;                break;
        case ERR_CFG_INVALID_PATH:       msg = "location path must start with '/': '" + detail + "'";    break;
        case ERR_CFG_INVALID_CGI_EXTENSION: msg = "cgi_extension must start with '.': '" + detail + "'"; break;
        case ERR_CFG_MISSING_CGI_PATH:   msg = "Missing interpreter path for cgi_extension " + detail;   break;
        default:                         msg = "Unknown config error";                               break;
    }
    throw std::runtime_error(msg);
}
