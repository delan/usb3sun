macro_rules! make_break_enum {
    ($(#[$meta:meta])* $vis:vis enum $name:ident { $($variant:ident = $make:expr,)+ }) => {
        ::paste::paste! {
            $(#[$meta])* $vis enum $name {
                $(
                    [<Make $variant>] = $make,
                    [<Break $variant>] = $make | 0x80,
                )+
            }
        }
    }
}

// to update holes:
// 1. `for i in {0..127}; do printf 'Hole%u = %u,\n' $i $i; done | xclip -sel clip`
// 2. `comm -23 <(seq 0 127 | sort) <(pio run -e linux 2>&1 | rg -o ' Hole[^ ]+ = [^,]+,' | cut -c 6- | cut -d' ' -f1 | sort -u)`

/// sun type 4, 107-key physical layout.
///
/// ```
/// 01h 03h 05h 06h 08h 0Ah 0Ch 0Eh 10h 11h 12h 07h 09h 0Bh 58h 42h 15h 16h 17h 62h
/// 19h 1Ah 1Dh 1Eh 1Fh 20h 21h 22h 23h 24h 25h 26h 27h 28h 29h 2Bh 2Dh 2Eh 2Fh 47h
/// 31h 33h 35h 36h 37h 38h 39h 3Ah 3Bh 3Ch 3Dh 3Eh 3Fh 40h 41h 44h 45h 46h
/// 48h 49h 4Ch 4Dh 4Eh 4Fh 50h 51h 52h 53h 54h 55h 56h 57h 2Ah 59h 5Bh 5Ch 5Dh 7Dh
/// 5Fh 61h 63h 64h 65h 66h 67h 68h 69h 6Ah 6Bh 6Ch 6Dh 6Eh 6Fh 70h 71h 72h
/// 76h 77h 13h 78h 79h 7Ah 43h 0Dh 5Eh 32h 5Ah
/// ```
macro_rules! type_4_107_key_layout {
    ($(#[$meta:meta])* $vis:vis enum $name:ident {
        $Key01h:ident $Key03h:ident $Key05h:ident $Key06h:ident $Key08h:ident $Key0Ah:ident $Key0Ch:ident $Key0Eh:ident $Key10h:ident $Key11h:ident $Key12h:ident $Key07h:ident $Key09h:ident $Key0Bh:ident $Key58h:ident $Key42h:ident $Key15h:ident $Key16h:ident $Key17h:ident $Key62h:ident
        $Key19h:ident $Key1Ah:ident $Key1Dh:ident $Key1Eh:ident $Key1Fh:ident $Key20h:ident $Key21h:ident $Key22h:ident $Key23h:ident $Key24h:ident $Key25h:ident $Key26h:ident $Key27h:ident $Key28h:ident $Key29h:ident $Key2Bh:ident $Key2Dh:ident $Key2Eh:ident $Key2Fh:ident $Key47h:ident
        $Key31h:ident $Key33h:ident $Key35h:ident $Key36h:ident $Key37h:ident $Key38h:ident $Key39h:ident $Key3Ah:ident $Key3Bh:ident $Key3Ch:ident $Key3Dh:ident $Key3Eh:ident $Key3Fh:ident $Key40h:ident $Key41h:ident $Key44h:ident $Key45h:ident $Key46h:ident
        $Key48h:ident $Key49h:ident $Key4Ch:ident $Key4Dh:ident $Key4Eh:ident $Key4Fh:ident $Key50h:ident $Key51h:ident $Key52h:ident $Key53h:ident $Key54h:ident $Key55h:ident $Key56h:ident $Key57h:ident $Key2Ah:ident $Key59h:ident $Key5Bh:ident $Key5Ch:ident $Key5Dh:ident $Key7Dh:ident
        $Key5Fh:ident $Key61h:ident $Key63h:ident $Key64h:ident $Key65h:ident $Key66h:ident $Key67h:ident $Key68h:ident $Key69h:ident $Key6Ah:ident $Key6Bh:ident $Key6Ch:ident $Key6Dh:ident $Key6Eh:ident $Key6Fh:ident $Key70h:ident $Key71h:ident $Key72h:ident
        $Key76h:ident $Key77h:ident $Key13h:ident $Key78h:ident $Key79h:ident $Key7Ah:ident $Key43h:ident $Key0Dh:ident $Key5Eh:ident $Key32h:ident $Key5Ah:ident
    }) => {
        make_break_enum! {
            $(#[$meta])* $vis enum $name {
                $Key01h = 0x01, $Key03h = 0x03, $Key05h = 0x05, $Key06h = 0x06, $Key08h = 0x08, $Key0Ah = 0x0A, $Key0Ch = 0x0C, $Key0Eh = 0x0E, $Key10h = 0x10, $Key11h = 0x11, $Key12h = 0x12, $Key07h = 0x07, $Key09h = 0x09, $Key0Bh = 0x0B, $Key58h = 0x58, $Key42h = 0x42, $Key15h = 0x15, $Key16h = 0x16, $Key17h = 0x17, $Key62h = 0x62,
                $Key19h = 0x19, $Key1Ah = 0x1A, $Key1Dh = 0x1D, $Key1Eh = 0x1E, $Key1Fh = 0x1F, $Key20h = 0x20, $Key21h = 0x21, $Key22h = 0x22, $Key23h = 0x23, $Key24h = 0x24, $Key25h = 0x25, $Key26h = 0x26, $Key27h = 0x27, $Key28h = 0x28, $Key29h = 0x29, $Key2Bh = 0x2B, $Key2Dh = 0x2D, $Key2Eh = 0x2E, $Key2Fh = 0x2F, $Key47h = 0x47,
                $Key31h = 0x31, $Key33h = 0x33, $Key35h = 0x35, $Key36h = 0x36, $Key37h = 0x37, $Key38h = 0x38, $Key39h = 0x39, $Key3Ah = 0x3A, $Key3Bh = 0x3B, $Key3Ch = 0x3C, $Key3Dh = 0x3D, $Key3Eh = 0x3E, $Key3Fh = 0x3F, $Key40h = 0x40, $Key41h = 0x41, $Key44h = 0x44, $Key45h = 0x45, $Key46h = 0x46,
                $Key48h = 0x48, $Key49h = 0x49, $Key4Ch = 0x4C, $Key4Dh = 0x4D, $Key4Eh = 0x4E, $Key4Fh = 0x4F, $Key50h = 0x50, $Key51h = 0x51, $Key52h = 0x52, $Key53h = 0x53, $Key54h = 0x54, $Key55h = 0x55, $Key56h = 0x56, $Key57h = 0x57, $Key2Ah = 0x2A, $Key59h = 0x59, $Key5Bh = 0x5B, $Key5Ch = 0x5C, $Key5Dh = 0x5D, $Key7Dh = 0x7D,
                $Key5Fh = 0x5F, $Key61h = 0x61, $Key63h = 0x63, $Key64h = 0x64, $Key65h = 0x65, $Key66h = 0x66, $Key67h = 0x67, $Key68h = 0x68, $Key69h = 0x69, $Key6Ah = 0x6A, $Key6Bh = 0x6B, $Key6Ch = 0x6C, $Key6Dh = 0x6D, $Key6Eh = 0x6E, $Key6Fh = 0x6F, $Key70h = 0x70, $Key71h = 0x71, $Key72h = 0x72,
                $Key76h = 0x76, $Key77h = 0x77, $Key13h = 0x13, $Key78h = 0x78, $Key79h = 0x79, $Key7Ah = 0x7A, $Key43h = 0x43, $Key0Dh = 0x0D, $Key5Eh = 0x5E, $Key32h = 0x32, $Key5Ah = 0x5A,
                Hole0 = 0, Hole2 = 2, Hole4 = 4, Hole15 = 15, Hole20 = 20, Hole24 = 24, Hole27 = 27, Hole28 = 28, Hole44 = 44, Hole48 = 48, Hole52 = 52, Hole74 = 74, Hole75 = 75, Hole96 = 96, Hole115 = 115, Hole116 = 116, Hole117 = 117, Hole123 = 123, Hole124 = 124, Hole126 = 126, Hole127 = 127,
            }
        }
    }
}

/// sun type 5, 118-key US/TW/KR physical layout.
///
/// ```
/// 118 29 5 6 8 10 12 14 16 17 18 7 9 11 22 23 21 45 2 4 48
/// 1 3 42 30 31 32 33 34 35 36 37 38 39 40 41 43 44 52 96 98 46 47 71
/// 25 26 53 54 55 56 57 58 59 60 61 62 63 64 65 88 66 74 123 68 69 70
/// 49 51 119 77 78 79 80 81 82 83 84 85 86 87 89 91 92 93 125
/// 72 73 99 100 101 102 103 104 105 106 107 108 109 110 20 112 113 114
/// 95 97 76 19 120 121 122 67 13 24 27 28 94 50 90
/// ```
macro_rules! type_5_us_tw_kr_layout {
    ($(#[$meta:meta])* $vis:vis enum $name:ident {
        $Key118:ident $Key29:ident $Key5:ident $Key6:ident $Key8:ident $Key10:ident $Key12:ident $Key14:ident $Key16:ident $Key17:ident $Key18:ident $Key7:ident $Key9:ident $Key11:ident $Key22:ident $Key23:ident $Key21:ident $Key45:ident $Key2:ident $Key4:ident $Key48:ident
        $Key1:ident $Key3:ident $Key42:ident $Key30:ident $Key31:ident $Key32:ident $Key33:ident $Key34:ident $Key35:ident $Key36:ident $Key37:ident $Key38:ident $Key39:ident $Key40:ident $Key41:ident $Key43:ident $Key44:ident $Key52:ident $Key96:ident $Key98:ident $Key46:ident $Key47:ident $Key71:ident
        $Key25:ident $Key26:ident $Key53:ident $Key54:ident $Key55:ident $Key56:ident $Key57:ident $Key58:ident $Key59:ident $Key60:ident $Key61:ident $Key62:ident $Key63:ident $Key64:ident $Key65:ident $Key88:ident $Key66:ident $Key74:ident $Key123:ident $Key68:ident $Key69:ident $Key70:ident
        $Key49:ident $Key51:ident $Key119:ident $Key77:ident $Key78:ident $Key79:ident $Key80:ident $Key81:ident $Key82:ident $Key83:ident $Key84:ident $Key85:ident $Key86:ident $Key87:ident $Key89:ident $Key91:ident $Key92:ident $Key93:ident $Key125:ident
        $Key72:ident $Key73:ident $Key99:ident $Key100:ident $Key101:ident $Key102:ident $Key103:ident $Key104:ident $Key105:ident $Key106:ident $Key107:ident $Key108:ident $Key109:ident $Key110:ident $Key20:ident $Key112:ident $Key113:ident $Key114:ident
        $Key95:ident $Key97:ident $Key76:ident $Key19:ident $Key120:ident $Key121:ident $Key122:ident $Key67:ident $Key13:ident $Key24:ident $Key27:ident $Key28:ident $Key94:ident $Key50:ident $Key90:ident
    }) => {
        make_break_enum! {
            $(#[$meta])* $vis enum $name {
                $Key118 = 118, $Key29 = 29, $Key5 = 5, $Key6 = 6, $Key8 = 8, $Key10 = 10, $Key12 = 12, $Key14 = 14, $Key16 = 16, $Key17 = 17, $Key18 = 18, $Key7 = 7, $Key9 = 9, $Key11 = 11, $Key22 = 22, $Key23 = 23, $Key21 = 21, $Key45 = 45, $Key2 = 2, $Key4 = 4, $Key48 = 48,
                $Key1 = 1, $Key3 = 3, $Key42 = 42, $Key30 = 30, $Key31 = 31, $Key32 = 32, $Key33 = 33, $Key34 = 34, $Key35 = 35, $Key36 = 36, $Key37 = 37, $Key38 = 38, $Key39 = 39, $Key40 = 40, $Key41 = 41, $Key43 = 43, $Key44 = 44, $Key52 = 52, $Key96 = 96, $Key98 = 98, $Key46 = 46, $Key47 = 47, $Key71 = 71,
                $Key25 = 25, $Key26 = 26, $Key53 = 53, $Key54 = 54, $Key55 = 55, $Key56 = 56, $Key57 = 57, $Key58 = 58, $Key59 = 59, $Key60 = 60, $Key61 = 61, $Key62 = 62, $Key63 = 63, $Key64 = 64, $Key65 = 65, $Key88 = 88, $Key66 = 66, $Key74 = 74, $Key123 = 123, $Key68 = 68, $Key69 = 69, $Key70 = 70,
                $Key49 = 49, $Key51 = 51, $Key119 = 119, $Key77 = 77, $Key78 = 78, $Key79 = 79, $Key80 = 80, $Key81 = 81, $Key82 = 82, $Key83 = 83, $Key84 = 84, $Key85 = 85, $Key86 = 86, $Key87 = 87, $Key89 = 89, $Key91 = 91, $Key92 = 92, $Key93 = 93, $Key125 = 125,
                $Key72 = 72, $Key73 = 73, $Key99 = 99, $Key100 = 100, $Key101 = 101, $Key102 = 102, $Key103 = 103, $Key104 = 104, $Key105 = 105, $Key106 = 106, $Key107 = 107, $Key108 = 108, $Key109 = 109, $Key110 = 110, $Key20 = 20, $Key112 = 112, $Key113 = 113, $Key114 = 114,
                $Key95 = 95, $Key97 = 97, $Key76 = 76, $Key19 = 19, $Key120 = 120, $Key121 = 121, $Key122 = 122, $Key67 = 67, $Key13 = 13, $Key24 = 24, $Key27 = 27, $Key28 = 28, $Key94 = 94, $Key50 = 50, $Key90 = 90,
                Hole0 = 0, Hole15 = 15, Hole75 = 75, Hole111 = 111, Hole115 = 115, Hole116 = 116, Hole117 = 117, Hole124 = 124, Hole126 = 126, Hole127 = 127,
            }
        }
    }
}

/// sun type 5, 119-key “UNIX” physical layout.
///
/// ```
/// 118 15 5 6 8 10 12 14 16 17 18 7 9 11 22 23 21 45 2 4 48
/// 1 3 29 30 31 32 33 34 35 36 37 38 39 40 41 88 42 44 52 96 98 46 47 71
/// 25 26 53 54 55 56 57 58 59 60 61 62 63 64 65 43 66 74 123 68 69 70
/// 49 51 76 77 78 79 80 81 82 83 84 85 86 87 89 91 92 93 125
/// 72 73 99 100 101 102 103 104 105 106 107 108 109 110 20 112 113 114
/// 95 97 119 19 120 121 122 67 13 24 27 28 94 50 90
/// ```
macro_rules! type_5_unix_layout {
    ($(#[$meta:meta])* $vis:vis enum $name:ident {
        $Key118:ident $Key15:ident $Key5:ident $Key6:ident $Key8:ident $Key10:ident $Key12:ident $Key14:ident $Key16:ident $Key17:ident $Key18:ident $Key7:ident $Key9:ident $Key11:ident $Key22:ident $Key23:ident $Key21:ident $Key45:ident $Key2:ident $Key4:ident $Key48:ident
        $Key1:ident $Key3:ident $Key29:ident $Key30:ident $Key31:ident $Key32:ident $Key33:ident $Key34:ident $Key35:ident $Key36:ident $Key37:ident $Key38:ident $Key39:ident $Key40:ident $Key41:ident $Key88:ident $Key42:ident $Key44:ident $Key52:ident $Key96:ident $Key98:ident $Key46:ident $Key47:ident $Key71:ident
        $Key25:ident $Key26:ident $Key53:ident $Key54:ident $Key55:ident $Key56:ident $Key57:ident $Key58:ident $Key59:ident $Key60:ident $Key61:ident $Key62:ident $Key63:ident $Key64:ident $Key65:ident $Key43:ident $Key66:ident $Key74:ident $Key123:ident $Key68:ident $Key69:ident $Key70:ident
        $Key49:ident $Key51:ident $Key76:ident $Key77:ident $Key78:ident $Key79:ident $Key80:ident $Key81:ident $Key82:ident $Key83:ident $Key84:ident $Key85:ident $Key86:ident $Key87:ident $Key89:ident $Key91:ident $Key92:ident $Key93:ident $Key125:ident
        $Key72:ident $Key73:ident $Key99:ident $Key100:ident $Key101:ident $Key102:ident $Key103:ident $Key104:ident $Key105:ident $Key106:ident $Key107:ident $Key108:ident $Key109:ident $Key110:ident $Key20:ident $Key112:ident $Key113:ident $Key114:ident
        $Key95:ident $Key97:ident $Key119:ident $Key19:ident $Key120:ident $Key121:ident $Key122:ident $Key67:ident $Key13:ident $Key24:ident $Key27:ident $Key28:ident $Key94:ident $Key50:ident $Key90:ident
    }) => {
        make_break_enum! {
            $(#[$meta])* $vis enum $name {
                $Key118 = 118, $Key15 = 15, $Key5 = 5, $Key6 = 6, $Key8 = 8, $Key10 = 10, $Key12 = 12, $Key14 = 14, $Key16 = 16, $Key17 = 17, $Key18 = 18, $Key7 = 7, $Key9 = 9, $Key11 = 11, $Key22 = 22, $Key23 = 23, $Key21 = 21, $Key45 = 45, $Key2 = 2, $Key4 = 4, $Key48 = 48,
                $Key1 = 1, $Key3 = 3, $Key29 = 29, $Key30 = 30, $Key31 = 31, $Key32 = 32, $Key33 = 33, $Key34 = 34, $Key35 = 35, $Key36 = 36, $Key37 = 37, $Key38 = 38, $Key39 = 39, $Key40 = 40, $Key41 = 41, $Key88 = 88, $Key42 = 42, $Key44 = 44, $Key52 = 52, $Key96 = 96, $Key98 = 98, $Key46 = 46, $Key47 = 47, $Key71 = 71,
                $Key25 = 25, $Key26 = 26, $Key53 = 53, $Key54 = 54, $Key55 = 55, $Key56 = 56, $Key57 = 57, $Key58 = 58, $Key59 = 59, $Key60 = 60, $Key61 = 61, $Key62 = 62, $Key63 = 63, $Key64 = 64, $Key65 = 65, $Key43 = 43, $Key66 = 66, $Key74 = 74, $Key123 = 123, $Key68 = 68, $Key69 = 69, $Key70 = 70,
                $Key49 = 49, $Key51 = 51, $Key76 = 76, $Key77 = 77, $Key78 = 78, $Key79 = 79, $Key80 = 80, $Key81 = 81, $Key82 = 82, $Key83 = 83, $Key84 = 84, $Key85 = 85, $Key86 = 86, $Key87 = 87, $Key89 = 89, $Key91 = 91, $Key92 = 92, $Key93 = 93, $Key125 = 125,
                $Key72 = 72, $Key73 = 73, $Key99 = 99, $Key100 = 100, $Key101 = 101, $Key102 = 102, $Key103 = 103, $Key104 = 104, $Key105 = 105, $Key106 = 106, $Key107 = 107, $Key108 = 108, $Key109 = 109, $Key110 = 110, $Key20 = 20, $Key112 = 112, $Key113 = 113, $Key114 = 114,
                $Key95 = 95, $Key97 = 97, $Key119 = 119, $Key19 = 19, $Key120 = 120, $Key121 = 121, $Key122 = 122, $Key67 = 67, $Key13 = 13, $Key24 = 24, $Key27 = 27, $Key28 = 28, $Key94 = 94, $Key50 = 50, $Key90 = 90,
                Hole0 = 0, Hole111 = 111, Hole115 = 115, Hole116 = 116, Hole117 = 117, Hole124 = 124, Hole126 = 126, Hole127 = 127, Hole75 = 75,
            }
        }
    }
}

/// sun type 5, 119-key “TUV-Compliant” physical layout.
///
/// ```
/// 118 29 5 6 8 10 12 14 16 17 18 7 9 11 22 23 21 45 2 4 48
/// 1 3 42 30 31 32 33 34 35 36 37 38 39 40 41 43 44 52 96 98 46 47 71
/// 25 26 53 54 55 56 57 58 59 60 61 62 63 64 65 66 74 123 68 69 70
/// 49 51 119 77 78 79 80 81 82 83 84 85 86 87 88 89 91 92 93 125
/// 72 73 99 124 100 101 102 103 104 105 106 107 108 109 110 20 112 113 114
/// 95 97 76 19 120 121 122 67 13 24 27 28 94 50 90
/// ```
macro_rules! type_5_tuv_compliant_layout {
    ($(#[$meta:meta])* $vis:vis enum $name:ident {
        $Key118:ident $Key29:ident $Key5:ident $Key6:ident $Key8:ident $Key10:ident $Key12:ident $Key14:ident $Key16:ident $Key17:ident $Key18:ident $Key7:ident $Key9:ident $Key11:ident $Key22:ident $Key23:ident $Key21:ident $Key45:ident $Key2:ident $Key4:ident $Key48:ident
        $Key1:ident $Key3:ident $Key42:ident $Key30:ident $Key31:ident $Key32:ident $Key33:ident $Key34:ident $Key35:ident $Key36:ident $Key37:ident $Key38:ident $Key39:ident $Key40:ident $Key41:ident $Key43:ident $Key44:ident $Key52:ident $Key96:ident $Key98:ident $Key46:ident $Key47:ident $Key71:ident
        $Key25:ident $Key26:ident $Key53:ident $Key54:ident $Key55:ident $Key56:ident $Key57:ident $Key58:ident $Key59:ident $Key60:ident $Key61:ident $Key62:ident $Key63:ident $Key64:ident $Key65:ident $Key66:ident $Key74:ident $Key123:ident $Key68:ident $Key69:ident $Key70:ident
        $Key49:ident $Key51:ident $Key119:ident $Key77:ident $Key78:ident $Key79:ident $Key80:ident $Key81:ident $Key82:ident $Key83:ident $Key84:ident $Key85:ident $Key86:ident $Key87:ident $Key88:ident $Key89:ident $Key91:ident $Key92:ident $Key93:ident $Key125:ident
        $Key72:ident $Key73:ident $Key99:ident $Key124:ident $Key100:ident $Key101:ident $Key102:ident $Key103:ident $Key104:ident $Key105:ident $Key106:ident $Key107:ident $Key108:ident $Key109:ident $Key110:ident $Key20:ident $Key112:ident $Key113:ident $Key114:ident
        $Key95:ident $Key97:ident $Key76:ident $Key19:ident $Key120:ident $Key121:ident $Key122:ident $Key67:ident $Key13:ident $Key24:ident $Key27:ident $Key28:ident $Key94:ident $Key50:ident $Key90:ident
    }) => {
        make_break_enum! {
            $(#[$meta])* $vis enum $name {
                $Key118 = 118, $Key29 = 29, $Key5 = 5, $Key6 = 6, $Key8 = 8, $Key10 = 10, $Key12 = 12, $Key14 = 14, $Key16 = 16, $Key17 = 17, $Key18 = 18, $Key7 = 7, $Key9 = 9, $Key11 = 11, $Key22 = 22, $Key23 = 23, $Key21 = 21, $Key45 = 45, $Key2 = 2, $Key4 = 4, $Key48 = 48,
                $Key1 = 1, $Key3 = 3, $Key42 = 42, $Key30 = 30, $Key31 = 31, $Key32 = 32, $Key33 = 33, $Key34 = 34, $Key35 = 35, $Key36 = 36, $Key37 = 37, $Key38 = 38, $Key39 = 39, $Key40 = 40, $Key41 = 41, $Key43 = 43, $Key44 = 44, $Key52 = 52, $Key96 = 96, $Key98 = 98, $Key46 = 46, $Key47 = 47, $Key71 = 71,
                $Key25 = 25, $Key26 = 26, $Key53 = 53, $Key54 = 54, $Key55 = 55, $Key56 = 56, $Key57 = 57, $Key58 = 58, $Key59 = 59, $Key60 = 60, $Key61 = 61, $Key62 = 62, $Key63 = 63, $Key64 = 64, $Key65 = 65, $Key66 = 66, $Key74 = 74, $Key123 = 123, $Key68 = 68, $Key69 = 69, $Key70 = 70,
                $Key49 = 49, $Key51 = 51, $Key119 = 119, $Key77 = 77, $Key78 = 78, $Key79 = 79, $Key80 = 80, $Key81 = 81, $Key82 = 82, $Key83 = 83, $Key84 = 84, $Key85 = 85, $Key86 = 86, $Key87 = 87, $Key88 = 88, $Key89 = 89, $Key91 = 91, $Key92 = 92, $Key93 = 93, $Key125 = 125,
                $Key72 = 72, $Key73 = 73, $Key99 = 99, $Key124 = 124, $Key100 = 100, $Key101 = 101, $Key102 = 102, $Key103 = 103, $Key104 = 104, $Key105 = 105, $Key106 = 106, $Key107 = 107, $Key108 = 108, $Key109 = 109, $Key110 = 110, $Key20 = 20, $Key112 = 112, $Key113 = 113, $Key114 = 114,
                $Key95 = 95, $Key97 = 97, $Key76 = 76, $Key19 = 19, $Key120 = 120, $Key121 = 121, $Key122 = 122, $Key67 = 67, $Key13 = 13, $Key24 = 24, $Key27 = 27, $Key28 = 28, $Key94 = 94, $Key50 = 50, $Key90 = 90,
                Hole0 = 0, Hole15 = 15, Hole75 = 75, Hole111 = 111, Hole115 = 115, Hole116 = 116, Hole117 = 117, Hole126 = 126, Hole127 = 127,
            }
        }
    }
}

/// sun type 5, 122-key JP physical layout.
///
/// ```
/// 118 29 5 6 8 10 12 14 16 17 18 7 9 11 22 23 21 45 2 4 48
/// 1 3 42 30 31 32 33 34 35 36 37 38 39 40 41 43 44 52 96 98 46 47 71
/// 25 26 53 54 55 56 57 58 59 60 61 62 63 64 65 66 74 123 68 69 70
/// 49 51 119 77 78 79 80 81 82 83 84 85 86 87 88 89 91 92 93 125
/// 72 73 99 100 101 102 103 104 105 106 107 108 109 111 110 20 112 113 114
/// 95 97 76 19 120 115 121 116 117 122 67 13 24 27 28 94 50 90
/// ```
macro_rules! type_5_jp_layout {
    ($(#[$meta:meta])* $vis:vis enum $name:ident {
        $Key118:ident $Key29:ident $Key5:ident $Key6:ident $Key8:ident $Key10:ident $Key12:ident $Key14:ident $Key16:ident $Key17:ident $Key18:ident $Key7:ident $Key9:ident $Key11:ident $Key22:ident $Key23:ident $Key21:ident $Key45:ident $Key2:ident $Key4:ident $Key48:ident
        $Key1:ident $Key3:ident $Key42:ident $Key30:ident $Key31:ident $Key32:ident $Key33:ident $Key34:ident $Key35:ident $Key36:ident $Key37:ident $Key38:ident $Key39:ident $Key40:ident $Key41:ident $Key43:ident $Key44:ident $Key52:ident $Key96:ident $Key98:ident $Key46:ident $Key47:ident $Key71:ident
        $Key25:ident $Key26:ident $Key53:ident $Key54:ident $Key55:ident $Key56:ident $Key57:ident $Key58:ident $Key59:ident $Key60:ident $Key61:ident $Key62:ident $Key63:ident $Key64:ident $Key65:ident $Key66:ident $Key74:ident $Key123:ident $Key68:ident $Key69:ident $Key70:ident
        $Key49:ident $Key51:ident $Key119:ident $Key77:ident $Key78:ident $Key79:ident $Key80:ident $Key81:ident $Key82:ident $Key83:ident $Key84:ident $Key85:ident $Key86:ident $Key87:ident $Key88:ident $Key89:ident $Key91:ident $Key92:ident $Key93:ident $Key125:ident
        $Key72:ident $Key73:ident $Key99:ident $Key100:ident $Key101:ident $Key102:ident $Key103:ident $Key104:ident $Key105:ident $Key106:ident $Key107:ident $Key108:ident $Key109:ident $Key111:ident $Key110:ident $Key20:ident $Key112:ident $Key113:ident $Key114:ident
        $Key95:ident $Key97:ident $Key76:ident $Key19:ident $Key120:ident $Key115:ident $Key121:ident $Key116:ident $Key117:ident $Key122:ident $Key67:ident $Key13:ident $Key24:ident $Key27:ident $Key28:ident $Key94:ident $Key50:ident $Key90:ident
    }) => {
        make_break_enum! {
            $(#[$meta])* $vis enum $name {
                $Key118 = 118, $Key29 = 29, $Key5 = 5, $Key6 = 6, $Key8 = 8, $Key10 = 10, $Key12 = 12, $Key14 = 14, $Key16 = 16, $Key17 = 17, $Key18 = 18, $Key7 = 7, $Key9 = 9, $Key11 = 11, $Key22 = 22, $Key23 = 23, $Key21 = 21, $Key45 = 45, $Key2 = 2, $Key4 = 4, $Key48 = 48,
                $Key1 = 1, $Key3 = 3, $Key42 = 42, $Key30 = 30, $Key31 = 31, $Key32 = 32, $Key33 = 33, $Key34 = 34, $Key35 = 35, $Key36 = 36, $Key37 = 37, $Key38 = 38, $Key39 = 39, $Key40 = 40, $Key41 = 41, $Key43 = 43, $Key44 = 44, $Key52 = 52, $Key96 = 96, $Key98 = 98, $Key46 = 46, $Key47 = 47, $Key71 = 71,
                $Key25 = 25, $Key26 = 26, $Key53 = 53, $Key54 = 54, $Key55 = 55, $Key56 = 56, $Key57 = 57, $Key58 = 58, $Key59 = 59, $Key60 = 60, $Key61 = 61, $Key62 = 62, $Key63 = 63, $Key64 = 64, $Key65 = 65, $Key66 = 66, $Key74 = 74, $Key123 = 123, $Key68 = 68, $Key69 = 69, $Key70 = 70,
                $Key49 = 49, $Key51 = 51, $Key119 = 119, $Key77 = 77, $Key78 = 78, $Key79 = 79, $Key80 = 80, $Key81 = 81, $Key82 = 82, $Key83 = 83, $Key84 = 84, $Key85 = 85, $Key86 = 86, $Key87 = 87, $Key88 = 88, $Key89 = 89, $Key91 = 91, $Key92 = 92, $Key93 = 93, $Key125 = 125,
                $Key72 = 72, $Key73 = 73, $Key99 = 99, $Key100 = 100, $Key101 = 101, $Key102 = 102, $Key103 = 103, $Key104 = 104, $Key105 = 105, $Key106 = 106, $Key107 = 107, $Key108 = 108, $Key109 = 109, $Key111 = 111, $Key110 = 110, $Key20 = 20, $Key112 = 112, $Key113 = 113, $Key114 = 114,
                $Key95 = 95, $Key97 = 97, $Key76 = 76, $Key19 = 19, $Key120 = 120, $Key115 = 115, $Key121 = 121, $Key116 = 116, $Key117 = 117, $Key122 = 122, $Key67 = 67, $Key13 = 13, $Key24 = 24, $Key27 = 27, $Key28 = 28, $Key94 = 94, $Key50 = 50, $Key90 = 90,
                Hole0 = 0, Hole15 = 15, Hole75 = 75, Hole124 = 124, Hole126 = 126, Hole127 = 127,
            }
        }
    }
}

type_4_107_key_layout! {
    /// sun type 4, en-US layout, using the 107-key physical layout.
    #[repr(u8)]
    pub enum Type4EnUs {
        StopL1 AgainL2 F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 BackslashPipe Delete
        PauseR1 PrtScR2 ScrollLockBreakR3 NumLock

        PropsL3 UndoL4 Esc Main1 Main2 Main3 Main4 Main5 Main6 Main7 Main8 Main9
        Main0 DashUnderscore EqualsPlus BackSpace NumpadEqualsR4 NumpadDivideR5
        NumpadTimesR6 NumpadMinus

        FrontL5 CopyL6 Tab LetterQ LetterW LetterE LetterR LetterT LetterY
        LetterU LetterI LetterO LetterP LeftBracketBrace RightBracketBrace
        NumpadHome7R7 NumpadUp8R8 NumpadPgUp9R9

        OpenL7 PasteL8 Control LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL SemicolonColon SingleDoubleQuote BacktickTilde
        Return NumpadLeft4R10 Numpad5R11 NumpadRight6R12 NumpadPlus

        FindL9 CutL10 LeftShift LetterZ LetterX LetterC LetterV LetterB LetterN
        LetterM CommaLessThan PeriodGreaterThan SlashQuestion RightShift
        LineFeed NumpadEnd1R13 NumpadDown2R14 NumpadPgDn3R15

        Help CapsLock Alt LeftMeta Space RightMeta Compose AltGraph NumpadIns0
        NumpadPointDel NumpadEnter
    }
}

type_5_us_tw_kr_layout! {
    /// sun type 5, en-US layout, using the 118-key US/TW/KR physical layout.
    #[repr(u8)]
    pub enum Type5EnUs {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again BacktickTilde OneExclamation TwoAt ThreeHash FourDollar
        FivePercent SixCaret SevenAmpersand EightAsterisk NineLeftParen
        ZeroRightParen DashUnderscore EqualsPlus BackSpace Insert Home PageUp
        NumLock NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterQ LetterW LetterE LetterR LetterT LetterY LetterU
        LetterI LetterO LetterP LeftBracketBrace RightBracketBrace BackslashPipe
        Del End PageDown NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy CapsLock LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL SemicolonColon SingleDoubleQuote Enter
        NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift LetterZ LetterX LetterC LetterV LetterB LetterN
        LetterM CommaLessThan PeriodGreaterThan SlashQuestion RightShift
        UpArrow NumpadEnd1 NumpadDown2 NumpadPgDn3

        Find Cut Control Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_unix_layout! {
    /// sun type 5, unix layout (?en-US), using the 119-key “UNIX” physical layout.
    #[repr(u8)]
    pub enum Type5Unix {
        Help Blank F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again Esc OneExclamation TwoAt ThreeHash FourDollar FivePercent
        SixCaret SevenAmpersand EightAsterisk NineLeftParen ZeroRightParen
        DashUnderscore EqualsPlus BackslashPipe BacktickTilde Insert Home PageUp
        NumLock NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterQ LetterW LetterE LetterR LetterT LetterY LetterU
        LetterI LetterO LetterP LeftBracketBrace RightBracketBrace BackSpace Del
        End PageDown NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy Control LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL SemicolonColon SingleDoubleQuote Return
        NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift LetterZ LetterX LetterC LetterV LetterB LetterN
        LetterM CommaLessThan PeriodGreaterThan SlashQuestion RightShift UpArrow
        NumpadEnd1 NumpadDown2 NumpadPgDn3

        Find Cut CapsLock Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_tuv_compliant_layout! {
    /// sun type 5, de-DE layout, using the 119-key “TUV-Compliant” physical layout.
    #[repr(u8)]
    pub enum Type5DeDe {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again CaretDegrees OneExclamation TwoDoubleQuoteSquared
        ThreeSectionCubed FourDollar FivePercent SixAmpersand
        SevenSlashLeftBrace EightLeftParenBracket NineRightParenBracket
        ZeroEqualsRightBrace EszettQuestionBackslash DeadAcuteGrave BackSpace
        Insert Home PageUp NumLock NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterQAt LetterW LetterE LetterR LetterT LetterZ LetterU
        LetterI LetterO LetterP LetterÜ PlusAsteriskTilde Del End PageDown
        NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy ShiftLock LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL LetterÖ LetterÄ HashSingleQuoteBacktick Return
        NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift LessGreaterPipe LetterY LetterX LetterC LetterV
        LetterB LetterN LetterMMicro CommaSemicolon PeriodColon DashUnderscore
        RightShift UpArrow NumpadEnd1 NumpadDown2 NumpadPgDn3

        Find Cut Control Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_tuv_compliant_layout! {
    /// sun type 5, fr-FR layout, using the 119-key “TUV-Compliant” physical layout.
    #[repr(u8)]
    pub enum Type5FrFr {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again SquaredCubed AmpersandOnePipe LetterÉTwoAt
        DoubleQuoteThreeHash SingleQuoteFour LeftParenFive SectionSixCaret
        LetterÈSeven ExclamationEight LetterÇNineLeftBrace LetterÀZeroRightBrace
        RightParenDegree DashUnderscore BackSpace Insert Home PageUp NumLock
        NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterA LetterZ LetterE LetterR LetterT LetterY LetterU
        LetterI LetterO LetterP DeadCircumflexDiaeresisLeftBracket
        DollarAsteriskRightBracket Del End PageDown NumpadHome7 NumpadUp8
        NumpadPgUp9

        Front Copy CapsLock /* ← ?ShiftLock */ LetterQ LetterS LetterD LetterF
        LetterG LetterH LetterJ LetterK LetterL LetterM LetterÙPercentDeadAcute
        MicroPoundDeadGrave Return NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift LessGreaterBackslash LetterW LetterX LetterC
        LetterV LetterB LetterN CommaQuestion SemicolonPeriod ColonSlash
        EqualsPlusDeadTilde RightShift UpArrow NumpadEnd1 NumpadDown2
        NumpadPgDn3

        Find Cut Control Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_tuv_compliant_layout! {
    /// sun type 5, en-UK layout, using the 119-key “TUV-Compliant” physical layout.
    #[repr(u8)]
    pub enum Type5EnUk {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again BacktickNotBrokenBar OneExclamation TwoDoubleQuote ThreePound
        FourDollar FivePercent SixCaret SevenAmpersand EightAsterisk
        NineLeftParen ZeroRightParen DashUnderscore EqualsPlus BackSpace Insert
        Home PageUp NumLock NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterQ LetterW LetterE LetterR LetterT LetterY LetterU
        LetterI LetterO LetterP LeftBracketBrace RightBracketBrace Del End
        PageDown NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy CapsLock LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL SemicolonColon SingleQuoteAt HashTilde Return
        NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift BackslashPipe LetterZ LetterX LetterC LetterV
        LetterB LetterN LetterM CommaLessThan PeriodGreaterThan SlashQuestion
        RightShift UpArrow NumpadEnd1 NumpadDown2 NumpadPgDn3

        Find Cut Control Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_tuv_compliant_layout! {
    /// sun type 5, sv-SE layout, using the 119-key “TUV-Compliant” physical layout.
    #[repr(u8)]
    pub enum Type5SvSe {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again SectionOneHalf OneExclamation TwoDoubleQuoteAt ThreeHashPound
        FourCurrencyDollar FivePercent SixAmpersand SevenSlashLeftBrace
        EightLeftParenBracket NineRightParenBracket ZeroEqualsRightBrace
        PlusQuestionBackslash DeadAcuteGrave BackSpace Insert Home PageUp
        NumLock NumpadDivide NumpadTimes NumpadMinus

        Props Undo Tab LetterQ LetterW LetterE LetterR LetterT LetterY LetterU
        LetterI LetterO LetterP LetterÅ DeadDiaeresisCircumflexTilde Del End
        PageDown NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy CapsLock LetterA LetterS LetterD LetterF LetterG LetterH
        LetterJ LetterK LetterL LetterÖ LetterÄ SingleQuoteAsteriskBacktick
        Return NumpadLeft4 Numpad5 NumpadRight6 NumpadPlus

        Open Paste LeftShift LessGreaterPipe LetterZ LetterX LetterC LetterV
        LetterB LetterN LetterM CommaSemicolon PeriodColon DashUnderscore
        RightShift UpArrow NumpadEnd1 NumpadDown2 NumpadPgDn3

        Find Cut Control Alt LeftMeta Space RightMeta Compose AltGraph LeftArrow
        DownArrow RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}

type_5_jp_layout! {
    /// sun type 5, jp-JP layout, using the 122-key JP physical layout.
    #[repr(u8)]
    pub enum Type5JpJp {
        Help Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 PrintScreenSysRq
        ScrollLock PauseBreak MuteDegauss DecreaseVolumeOrContrast
        IncreaseVolumeOrContrast Power

        Stop Again YenPipeDash OneExclamationぬ TwoDoubleQuoteふ ThreeHashあ
        FourDollarう FivePercentえ SixAmpersandお SevenSingleQuoteや
        EightLeftParenゆ NineRightParenよ Zeroわを DashEqualsほ CaretTildeへ
        BackSpace Insert Home PageUp NumLock NumpadDivide NumpadTimes
        NumpadMinus

        Props Undo Tab LetterQた LetterWて LetterEい LetterRす LetterTか
        LetterYん LetterUな LetterIに LetterOら LetterPせ AtBacktickDeadVoiced
        LeftBracketBraceDeadSemiVoicedLeftCorner Del End
        PageDown NumpadHome7 NumpadUp8 NumpadPgUp9

        Front Copy CapsLock LetterAち LetterSと LetterDし LetterFは LetterGき
        LetterHく LetterJま LetterKの LetterLり SemicolonPlusれ ColonAsteriskけ
        RightBracketBraceむRightCorner Return NumpadLeft4 Numpad5 NumpadRight6
        NumpadPlus

        Open Paste LeftShift LetterZつ LetterXさ LetterCそ LetterVひ LetterBこ
        LetterNみ LetterMも CommaLessThanねIdeographicComma
        PeriodGreaterThanるIdeographicFullStop SlashQuestionめInterpunct
        BackslashUnderscoreろ RightShift UpArrow NumpadEnd1 NumpadDown2
        NumpadPgDn3

        Find Cut Control Alt LeftMeta Kakutei確定 Space Henkan変換
        NihongoOnOff日本語 RightMeta Compose Kanaかな LeftArrow DownArrow
        RightArrow NumpadIns0 NumpadPointDel NumpadEnter
    }
}
