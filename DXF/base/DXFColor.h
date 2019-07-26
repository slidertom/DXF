#ifndef __DXF_COLOR_H__
#define __DXF_COLOR_H__
#pragma once

// First 7 colors of Autocad ACI (Autocad Color Index) table.
enum class EDXFColor : short int
{
    EDXFColorRed     = 1,
    EDXFColorYellow  = 2,
    EDXFColorGreen   = 3,
    EDXFColorCyan    = 4,
    EDXFColorBlue    = 5,
    EDXFColorMagenta = 6,
    EDXFColorWhite   = 7,
};

#endif