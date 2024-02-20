/**
 * @file    version.h
 * @author  Deadline039
 * @brief   Software versioning
 * @version 0.1
 * @date    2024-02-20
 * @note    You should modify the verison when you are release.
 *          Include This file and use `get_version_major, get_version minor,
 *          get_version_patch, get_version_info` to get the software version
 *          number and release information.
 */

#ifndef __VERSION_H
#define __VERSION_H

#define VERSION_MAJOR  0 /* Major Version Number */
#define VERSION_MINJOR 1 /* Minjor Version Number */
#define VERSION_PATCH  0 /* Patch Number */

/* The information about this version, such as bug fix, new feature */
#define VERSION_INFO   ""

/**
 * @brief Get the major version number.
 *
 * @return `int` The major version number
 */
static inline int get_version_major(void) {
    return VERSION_MAJOR;
}

/**
 * @brief Get the minjor version number.
 *
 * @return `int` The minjor version number
 */
static inline int get_version_minjor(void) {
    return VERSION_MINJOR;
}

/**
 * @brief Get the patch version number.
 *
 * @return `int` The patch version number
 */
static inline int get_version_patch(void) {
    return VERSION_PATCH;
}

/**
 * @brief Get this version information.
 *
 * @return `const char *` The information of this version
 */
static inline const char *get_version_info(void) {
    return VERSION_INFO;
}

#endif /* __VERSION_H */
