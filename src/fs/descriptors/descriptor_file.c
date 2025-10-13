/*
    =========================
    DESCRIPTOR FILE STRUCTURE
    =========================
    /files/example/
    example.cfg

    setting1 = true         <- Setting Defininition
    setting2 = page.html    <- Setting Defininition

    [IGNORELIST]            <- Ignore Header
    hidden                  <- Ignore Entry
    passwords.txt           <- Ignore Entry

    ignore:


    for [IGNORELIST] we check and add
    each entry until we hit another header or end of file.
*/


typedef struct descriptor_file{
    int hidden;
} descriptor_t;

/*
/// @brief parses the descriptor file to a structure
/// @param descriptor_file ptr to the descriptor file allocation
/// @return descriptor structure
descriptor_t parse_descriptor_file(const char *descriptor_file){

}
*/