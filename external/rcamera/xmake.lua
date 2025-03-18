target("rcamera")
    set_kind("headeronly")  -- Marks this as a header-only library
    add_includedirs("include", {public = true})