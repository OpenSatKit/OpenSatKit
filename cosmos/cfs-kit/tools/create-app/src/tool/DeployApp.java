package tool;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;  
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.nio.channels.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

import app.Constants;

public class DeployApp {
	   
   private enum MakeRule {NONE, BUILD_APP, INSTALL_APP, CLEAN_APP, DEPEND_APP};
   
   private String appName     = "null";
   private String utPath      = "null";  // Full path to target unit test
   private String missionPath = "null";  // Full path to target mission
private Object Constants;
   
   /**
    * Copy application files from the unit development directories to the mission (where the
    * cFS has been deployed) directories and to the CARE directories. This assumes the boot directory 
    * is a subdirectory of the mission based directory. 
    * 
    * @param appName      Name of the app. Not the app directory name (i.e. _app appended) and by convention lower case.
    * @param missionPath  Complete absolute path to cFS mission base directory 
    * @param bootRelPath  Relative path from the missionPath to the mission boot directory.  
    * @param missionPath  Complete absolute path to CARE base directory
    * 
    */
   public DeployApp(String appName, String utPath, String missionPath, String bootRelPath, String carePath) {
		
	  this.appName      = appName;
	  this.utPath       = utPath;
	  this.missionPath  = missionPath;
	  
	  String appDirName = appName;
			  
      File utFswFile    = new File(utPath+app.Constants.PATH_SEP+appDirName+app.Constants.PATH_SEP+app.Constants.PATH_UNIT_FSW);
      File utTestFile   = new File(utPath+app.Constants.PATH_SEP+appDirName+app.Constants.PATH_SEP+app.Constants.PATH_UNIT_TEST);
      File utDeployFile = new File(utPath+app.Constants.PATH_SEP+appDirName+app.Constants.PATH_SEP+app.Constants.PATH_UNIT_DEPLOY);

      File careDbAppFile = new File(carePath+app.Constants.PATH_SEP+app.Constants.PATH_DB_APPS);
      
      File missionFswFile   = new File(missionPath+app.Constants.PATH_MISSION_APPS+app.Constants.PATH_SEP+appDirName);
      File missionIncFile   = new File(missionPath+app.Constants.PATH_MISSION_INC);
      File platformFile     = new File(missionPath+app.Constants.PATH_PLATFORM);
      File platformIncFile  = new File(missionPath+app.Constants.PATH_PLATFORM_INC);
      File platformAppFile  = new File(missionPath+app.Constants.PATH_PLATFORM+app.Constants.PATH_SEP+appDirName);
      File platformBootFile = new File(missionPath+bootRelPath);
   

      // Copy Files from Unit Test (developer folders) to deployed folders
      // TODO - Default table files should be in FSW source directory or filter crap out of UT before copy. UT directory often has extra table versions for testing
      try {
    	  
    	  copyFiles (utFswFile,    missionFswFile);                // Source files
    	  copyFiles (utTestFile,   platformBootFile, "tbl");       // Default table files
    	  copyFiles (utDeployFile, missionIncFile, "mission");     // Mission config files
    	  copyFiles (utDeployFile, platformIncFile, "platform");   // Platform config files
    	  copyFiles (utDeployFile, platformAppFile, "Make");       // App make file
    	  copyFiles (utDeployFile, careDbAppFile, "xml");          // App XML database file
    	  updatePlatformMakefile(platformFile,appDirName);
    	  
      } 
      catch (Exception e) {

          e.printStackTrace();
 	 	       
 	  } // End property file exception catch 
	 	   
   } // End DeployApp()
	
	
   /**
    * Copy files from the source directory to the destination directory. The  destination
    * directory is created if it doesn't exist.
    * 
    * @param srcDirectory  Directory containing files to be copied 
    * @param destDirectory Destination directory that the files are copied into
    * 
    */
   public void copyFiles(File srcDirectory , File destDirectory) throws IOException {

	  System.out.println("copyFiles(): " + srcDirectory.getAbsolutePath() + ", " + destDirectory.getAbsolutePath());
      if (srcDirectory.isDirectory()) {
         if (!destDirectory.exists()) {
            destDirectory.mkdir();
         }
        	/* NIO - Got access violation
       	    FileChannel src  = null;
    	    FileChannel dest = null;
    	    try {
    	       src  = new FileInputStream(srcDirectory).getChannel();
    	       dest = new FileOutputStream(destDirectory).getChannel();
    	       dest.transferFrom(src, 0, src.size());
    	    }
    	    finally {
    	       if(src != null) {
    	          src.close();
    	       }
    	       if(dest != null) {
    	          dest.close();
    	       }
    	    }
    	    */
            
         File[] files = srcDirectory.listFiles();
         for(File file:files) {
        	  
              InputStream in = new FileInputStream(file);
              OutputStream out = new FileOutputStream(destDirectory+app.Constants.PATH_SEP+file.getName());

              // Copy the bits from input stream to output stream
              byte[] buf = new byte[1024];
              int len;
              while ((len = in.read(buf)) > 0) {
                  out.write(buf, 0, len);
              }
              in.close();
              out.close();
          } // End file loop
            
      } // End if src is a directory
   } // End copyFiles()
   
   
   /**
    * Copy files from the source directory to the destination directory. The destination
    * directory is created if it doesn't exist. Only files containing the substring in 
    * their filename are copied.
    * 
    * @param srcDirectory  Directory containing files to be copied 
    * @param destDirectory Destination directory that the files are copied into
    * 
    */
   public void copyFiles(File srcDirectory , File destDirectory, String subString) throws IOException {

      System.out.println("copyFiles(): " + srcDirectory.getAbsolutePath() + ", " + destDirectory.getAbsolutePath());
      if (srcDirectory.isDirectory()) {
         if (!destDirectory.exists()) {
            destDirectory.mkdir();
         }
            
         File[] files = srcDirectory.listFiles();
         for(File file:files) {

            if (file.getName().indexOf(subString) >= 0) {
              
               InputStream in = new FileInputStream(file);
               OutputStream out = new FileOutputStream(destDirectory+app.Constants.PATH_SEP+file.getName());

               // Copy the bits from input stream to output stream
               byte[] buf = new byte[1024];
               int len;
               while ((len = in.read(buf)) > 0) {
                  out.write(buf, 0, len);
               }
               in.close();
               out.close();
              
            } // End if contains subString
          } // End file loop
            
      } // End if src is a directory
   } // End copyFiles()

   
   public void updatePlatformMakefile(File makeFileDirectory, String newApp) throws IOException {

      // Small file so no need to optimize logic
      
	  System.out.println("MakeFile: " + makeFileDirectory.getAbsolutePath());
	  String makeFileName = makeFileDirectory+app.Constants.PATH_SEP+app.Constants.MAKEFILE_FILE_NAME;
      File makeFile = new File(makeFileName);
      File savedMakeFile = new File(makeFileName+"_saved");

      if (makeFile.exists()) {

          // Save original Makefile
    	  InputStream in = new FileInputStream(makeFile);
          OutputStream out = new FileOutputStream(savedMakeFile);
          
          // Copy the bits from input stream to output stream
          byte[] buf = new byte[1024];
          int len;
          while ((len = in.read(buf)) > 0) {
              out.write(buf, 0, len);
          }
          in.close();
          out.close();

          // Add new app to original Makefile
          
          MakeRule currRule = MakeRule.BUILD_APP;
          
          try {

              BufferedReader orgMake = new BufferedReader(new FileReader(savedMakeFile));
              BufferedWriter newMake = new BufferedWriter(new FileWriter(makeFile));

              String line = orgMake.readLine();

              while (line != null) {
            	 
            	 // Order of Makefile rules is assumed
                 if (line.indexOf(app.Constants.MAKEFILE_DEPLOY_KEYWORD) >= 0) {
            	    switch (currRule) {
            	    case BUILD_APP:
            	    	newMake.write(app.Constants.MAKEFILE_RULE_BUILD_APPS_STR.replace(app.Constants.MAKEFILE_DEPLOY_APP_VAR, newApp));
            	    	currRule = MakeRule.CLEAN_APP;
            	    	break;
            	    case CLEAN_APP:
            	    	newMake.write(app.Constants.MAKEFILE_RULE_CLEAN_APPS_STR.replace(app.Constants.MAKEFILE_DEPLOY_APP_VAR, newApp));
            	    	currRule = MakeRule.INSTALL_APP;
            	    	break;
            	    case INSTALL_APP:
            	    	newMake.write(app.Constants.MAKEFILE_RULE_INSTALL_APPS_STR.replace(app.Constants.MAKEFILE_DEPLOY_APP_VAR, newApp));
            	    	currRule = MakeRule.DEPEND_APP;
            	    	break;
            	    case DEPEND_APP:
            	    	newMake.write(app.Constants.MAKEFILE_RULE_DEPEND_APPS_STR.replace(app.Constants.MAKEFILE_DEPLOY_APP_VAR, newApp));
            	    	currRule = MakeRule.NONE;
            	    	break;
            		}
            	 } // End if deploy keyword
                 newMake.write(line+"\n");
                 
                 line = orgMake.readLine();
                 
              } // End while line
              
              orgMake.close();
              newMake.close();
          }
          catch (Exception e) {

              e.printStackTrace();
     	 	       
     	  } // End property file exception catch 
          
      } // End if makefile exists
      else {
          System.out.println("MakeFile does not exist: " + makeFile.getAbsolutePath());
      }
   } // End updatePlatformMakefile()
   
   public String getAppName ()
   {
	   return appName;
   }
   public String getUnitTestPath ()
   {
	   return utPath;
   }
   public String getMissionPath ()
   {
	   return missionPath;
   }

    /**
     *  
     * TODO - Replace with Junit.
     * 
     * @param args
     * @throws Exception
     */
     public static void main(String [] args) throws Exception
     {
      
         Properties appConfig = new Properties();
    			  
    	      try {
    		          
    	         appConfig.loadFromXML(new FileInputStream(app.Constants.APP_PROP_FILE));

    	    	 DeployApp newApp = new DeployApp("demo", appConfig.getProperty(app.Constants.PROP_UNIT_DEV_DIR), 
	                        appConfig.getProperty(app.Constants.PROP_CFS_MISSION_DIR),
	                        appConfig.getProperty(app.Constants.PROP_CFS_REL_BOOT_DIR),
	                        appConfig.getProperty(app.Constants.PROP_CARE_BASE_DIR));
    		 	   
    	      } catch (Exception e) {

    	         e.printStackTrace();
    		 	       
    		  } // End property file exception catch 

         
     } // End main()

} // End class DeployApp
