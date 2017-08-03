################################################################################
# cFE File Class
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################


#
# Standard cFE File header structure definition
# - Typically CFE_FS_HDR_DESC_MAX_LEN = 32
#
# typedef struct
# {
#   uint32  ContentType;           /**< \brief Identifies the content type (='cFE1'=0x63464531)*/
#   uint32  SubType;               /**< \brief Type of \c ContentType, if necessary */
#                                  /**< Standard SubType definitions can be found \link #CFE_FS_ES_ERLOG_SUBTYPE here \endlink */
#   uint32  Length;                /**< \brief Length of primary header */
#   uint32  SpacecraftID;          /**< \brief Spacecraft that generated the file */
#   uint32  ProcessorID;           /**< \brief Processor that generated the file */
#   uint32  ApplicationID;         /**< \brief Application that generated the file */
#  
#   uint32  TimeSeconds;           /**< \brief File creation timestamp (seconds) */
#   uint32  TimeSubSeconds;        /**< \brief File creation timestamp (sub-seconds) */
#
#   char    Description[CFE_FS_HDR_DESC_MAX_LEN];       /**< \brief File description */
#
# } CFE_FS_Header_t;
