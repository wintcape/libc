/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file common/units.h
 * @brief Defines common measurement units.
 */
#ifndef UNITS_H
#define UNITS_H

#define GIBIBYTES(x) ( (x) * 1024ULL*1024ULL*1024ULL )
#define MEBIBYTES(x) ( (x) * 1024ULL*1024ULL )
#define KIBIBYTES(x) ( (x) * 1024ULL )

#define GIGABYTES(x) ( (x) * 1000ULL*1000ULL*1000ULL )
#define MEGABYTES(x) ( (x) * 1000ULL*1000ULL )
#define KILOBYTES(x) ( (x) * 1000ULL )

#endif  // UNITS_H