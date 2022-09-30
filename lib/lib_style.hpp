#pragma once

#include <unordered_map>

namespace style {
    std::unordered_map<std::string, std::string> style {
        { "reset",     "\u001b[0m"  },
        { "bold",      "\u001b[1m"  },
        { "italic",    "\u001b[3m"  },
        { "underline", "\u001b[4m"  },
        { "blink",     "\u001b[5m"  },
        { "bg",        "\u001b[7m"  },
        { "strike",    "\u001b[9m"  },
        { "black",     "\u001b[30m" },
        { "red",       "\u001b[31m" },
        { "green",     "\u001b[32m" },
        { "yellow",    "\u001b[33m" },
        { "blue",      "\u001b[34m" },
        { "magenta",   "\u001b[35m" },
        { "cyan",      "\u001b[36m" },
        { "white",     "\u001b[37m" },
    };
}