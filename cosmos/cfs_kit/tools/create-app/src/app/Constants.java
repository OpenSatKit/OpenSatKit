package app;

/**
 * 
 * Define constants for the tool package
 * 
 * @author David McComas
 *
 */
public class Constants {

   static final public String PATH_CWD = System.getProperty("user.dir");
   static final public String PATH_SEP = System.getProperty("file.separator"); 
   
   /*****************************************************************************
   ** Common strings
   */
   static final public String UNDEF_STR = "Undefined";
   
   /*****************************************************************************
   ** AppGen default directory paths and files
   */

   static final public String APP_PROP_FILE = PATH_CWD + PATH_SEP + "genapp-properties.xml";
   static final public String TMP_PROP_FILE = "genapp-template.xml";
   static final public String IMAGE_DIR     = PATH_CWD + PATH_SEP + "images" + PATH_SEP;
   static final public String TEMPLATE_DIR  = PATH_CWD + PATH_SEP + "templates";
   static final public String TARGET_CFS_DIR    = PATH_CWD + PATH_SEP + "target-test";
   static final public String TARGET_COSMOS_DIR = PATH_CWD + PATH_SEP + "target-test" + PATH_SEP + "cosmos-config";
   static final public String CFS_ICON_FILE = "cFS_logo_blue-_C100_M40_Y0_B15-300x220.png";
   
   // Paths relative to COMSOS config directory
   static final public String COSMOS_CONFIG_TARGETS_DIR = "targets" + PATH_SEP;
   static final public String COSMOS_CONFIG_TARGET_FILE = "tools" + PATH_SEP + "cmd_tlm_server" + PATH_SEP + "cmd_tlm_server.txt";

   /*****************************************************************************
   ** AppGen Property file keywords
   */

   static final public String PROP_APP_TEMPLATE_DIR      = "template-dir";
   static final public String PROP_APP_TARGET_CFS_DIR    = "target-cfs-dir";
   static final public String PROP_APP_TARGET_COSMOS_DIR = "target-cosmos-dir";
   static final public String PROP_TEMPLATE_VAR_PREFIX   = "template-var";

   /*****************************************************************************
   ** Template Property file keywords
   */

   static final public String PROP_TEMPLATE_TITLE   = "title";    
   static final public String PROP_TEMPLATE_VERSION = "version";    
   static final public String PROP_TEMPLATE_DESCR   = "descr";    

   // Prefix keywords must end in a dash because the remaining part of the keyword is used as a directory
   static final public String PROP_TEMPLATE_CFS_APP_DIR_PREFIX = "cfsapp-";    
   static final public String PROP_TEMPLATE_COSMOS_DIR_PREFIX  = "cosmos-";
   
   static final public String PROP_TEMPLATE_DIR_SEP = "/";      // Use a character not allowed in directory names

   /*****************************************************************************
   ** Template file 
   */
       
   static final public String TEMPLATE_COMMENT_START  = "/*##";
   static final public String TEMPLATE_KEYWORD_START  = "Start";
   static final public String TEMPLATE_COMMENT_END    = "##*/";
   static final public String TEMPLATE_KEYWORD_END    = "End";
      
   /*****************************************************************************
   ** cFS Kit tags used to delineate sections in cFS and COSMOS files that can
   * automatically be updated during installation 
   */

   static final public String CFS_KIT_TAG_START = "<cfs_kit>";
   static final public String CFS_KIT_TAG_END   = "</cfs_kit>";
   
//******** TODO - Cleanup remaining constants used by DeployApp or unused ***************
   
   static final public String PROP_UNIT_DEV_DIR        = "unit-dev-dir";  // Used by deploy app. Need to remove becuase GUI supplied
   //static final public String PROP_E_VAR_PREFIX = "template-var";
   
   // Paths relative to the unit test directory that are used to deploy an app
   static final public String PATH_UNIT_FSW    = "fsw";
   static final public String PATH_UNIT_DEPLOY = "deploy";
   static final public String PATH_UNIT_TEST   = "ut";
   static final public String PATH_DB_APPS     = "db-apps";
   
   // - Template Application constants
   static final public String DEF_PLATFORM = "pc-linux";

   // Paths relative to the CFS mission base
   static final public String PATH_MISSION_APPS = "apps";
   static final public String PATH_MISSION_INC  = "build"+PATH_SEP+"mission_inc";
   static final public String PATH_PLATFORM     = "build"+PATH_SEP+DEF_PLATFORM;
   static final public String PATH_PLATFORM_INC = "build"+PATH_SEP+DEF_PLATFORM+PATH_SEP+"inc";
   
   static final public String MAKEFILE_FILE_NAME             = "Makefile";
   static final public String MAKEFILE_DEPLOY_KEYWORD        = "deploy-app";  // Indicates where to insert app commands
   static final public String MAKEFILE_DEPLOY_APP_VAR        = "_app_";
   static final public String MAKEFILE_RULE_BUILD_APPS       = "apps::";
   static final public String MAKEFILE_RULE_BUILD_APPS_STR   = "\t$(MAKE) -C " + MAKEFILE_DEPLOY_APP_VAR + "\n";	
   static final public String MAKEFILE_RULE_CLEAN_APPS       = "cleanapps::";
   static final public String MAKEFILE_RULE_CLEAN_APPS_STR   = "\t$(MAKE) -C " + MAKEFILE_DEPLOY_APP_VAR + "  clean\n";
   static final public String MAKEFILE_RULE_INSTALL_APPS     = "installapps::";
   static final public String MAKEFILE_RULE_INSTALL_APPS_STR = "\t$(MAKE) -C " + MAKEFILE_DEPLOY_APP_VAR + "  install\n";	
   static final public String MAKEFILE_RULE_DEPEND_APPS      = "dependapps::";
   static final public String MAKEFILE_RULE_DEPEND_APPS_STR  = "\t$(MAKE) -C " + MAKEFILE_DEPLOY_APP_VAR + "  depend\n";
                              

   static final public String PROP_CFS_MISSION_DIR   = "cfs-mission-dir";   // Location of cFS deployed for a "mission" on same computer as CARE
   static final public String PROP_CFS_REL_BOOT_DIR  = "cfs-rel-boot-dir";  // Boot directory relative to cFS mission directory
   static final public String PROP_CFS_DEF_TFTP_DIR  = "cfs-def-tftp-dir";  // FSW directory path

   static final public String PROP_CARE_BASE_DIR       = "care-base-dir";
   
} // End class Constants

