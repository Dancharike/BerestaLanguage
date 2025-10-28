//
// Created by Denis on 28.10.2025.
//

#ifndef BERESTALANGUAGE_EXPORT_H
#define BERESTALANGUAGE_EXPORT_H

#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(BERESTA_BUILD_DLL)
        #define BERESTA_API __declspec(dllexport)
    #else
        #define BERESTA_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define BERESTA_API __attribute__((visibility("default")))
    #else
        #define BERESTA_API
    #endif
#endif

#endif //BERESTALANGUAGE_EXPORT_H
