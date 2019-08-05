package tool;

import java.io.BufferedReader;   

import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.File;
import java.io.IOException;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.List;
import java.util.ArrayList;

import app.Constants;

/**
 *  
 * Create a new application. The property file defines the directories and
 * files to be process
 * 
 * @author David McComas
 *
 */
public class CreateApp {


   private String targetPath = "null";  // Full path to new app target directory

   private AppMap appMap;
   private String templateAppDir;
   private String targetCfsAppDir;
   private String targetCosmosConfigDir;
   
   private boolean processedTemplateFile = false;
   private boolean updatedCosmosCmdTlmFile = false;
   
   /**
    * 
    * @param appMap    Map containing template variables with corresponding replacement variables
    */
   public CreateApp(AppMap appMap, String templateAppDir, 
                    String targetCfsAppDir, String targetCosmosConfigDir) {
		
      this.appMap = appMap;
      this.templateAppDir  = templateAppDir;
      this.targetCfsAppDir = targetCfsAppDir;
      this.targetCosmosConfigDir = targetCosmosConfigDir;
      
   } // End CreateApp()
	
	
   /*
   ** The property file defines the directories and files that need to be
   ** created for a new app. This method calls InstantiateTemplate for each valid
   ** template entry. 
   */
   public boolean ProcessTemplateFile(Properties templateProp, String appName) {
		  	  	  
  
     targetCfsAppDir = targetCfsAppDir + Constants.PATH_SEP + appName.toLowerCase() + Constants.PATH_SEP;
 	  String targetCosmosCmdTlmDir = targetCosmosConfigDir + Constants.COSMOS_CONFIG_TARGETS_DIR + appName.toUpperCase() + Constants.PATH_SEP;
 	  
     System.out.println("targetCfsAppDir: " + targetCfsAppDir);
     System.out.println("targetCosmosCmdTlmDir: " + targetCosmosCmdTlmDir);

     File newDir = new File(targetCfsAppDir);
     newDir.mkdirs();
     System.out.println("Created dir: " + newDir.toString());

 	 
	  /*
	   *  Loop through property file locating all template directory keys and
	   *  instantiate files.
	   */
	  
	  for(String key : templateProp.stringPropertyNames()) {
         
    	  if (key.startsWith(Constants.PROP_TEMPLATE_CFS_APP_DIR_PREFIX)) {
    		  
    	     InstantiateTemplate (templateProp, key, Constants.PROP_TEMPLATE_CFS_APP_DIR_PREFIX, targetCfsAppDir);
    		  
    	  } // End if dir keyword
    	  else if (key.startsWith(Constants.PROP_TEMPLATE_COSMOS_DIR_PREFIX)) {
    	     
           InstantiateTemplate (templateProp, key, Constants.PROP_TEMPLATE_COSMOS_DIR_PREFIX, targetCosmosCmdTlmDir);
    	     
           UpdateCosmosCmdTlmServer(appName);
           
    	  } // End if cosmos
    	  
	   } // End property loop
   			
	  return processedTemplateFile;
	        
	} // End ProcessTemplateFile()


   /*
   ** This method creates a directory for the new app, loops through each sub-directory, and 
   ** instantiates each file specified in the template entry. 
   */
   protected void InstantiateTemplate(Properties templateProp, String key, String KeyPrefix, String targetDir) {
   
      // Extract string after directory keyword & replace "-" with PATH_SEP
      String subDir = key.substring(KeyPrefix.length()); 
      subDir.replace(Constants.PROP_TEMPLATE_DIR_SEP, Constants.PATH_SEP);
      
      // Complete subdirectory path is specified in the key. Create missing
      // subdirectories along the path. Found mkdirs() that does it for me!
      File newSubDir = new File(targetDir+Constants.PATH_SEP+subDir);
      newSubDir.mkdirs();
      
      String templateSubDir = templateAppDir + subDir + Constants.PATH_SEP;
      
      String value = templateProp.getProperty(key);
        String [] files = value.split(",");
   
      for (String file: files) {
         
         InstantiateFile(templateSubDir, newSubDir.getAbsolutePath() + Constants.PATH_SEP , file, appMap.getTemplateVarMap());

      } // End file loop 
            
   } // End InstantiateTemplate()
   
    /*
    ** Since template files should be short no need for fancy optimized algorithms. Keep it simple and readable. 
    */
	protected void InstantiateFile(String templateDir, String newAppDir, String file, Map<String, String> templateVarMap) {
	
		System.out.println("Template Dir: " + templateDir + ", New App Dir: " + newAppDir);
		System.out.println("Instantiating file " + file);
		
		boolean skipComment = false;
		Set<String> templateVarSet = templateVarMap.keySet();

		// Instantiate file names
		String newAppFile = file;
		for(String templateVar : templateVarSet) {

			newAppFile = newAppFile.replace(templateVar, templateVarMap.get(templateVar));

		} // End template variable loop
		
		try {
			
			FileReader templateFileRd = new FileReader(templateDir+file);
			BufferedReader fileReader = new BufferedReader(templateFileRd);
			
			FileWriter newAppFileWr = new FileWriter(newAppDir+newAppFile);
			BufferedWriter fileWriter = new BufferedWriter(newAppFileWr);
			
			String txtLine;
	        
			while ( (txtLine = fileReader.readLine()) != null ) {
				
		        if (skipComment) {
		        	
				   // Continue skipping until end of comment. Assume nothing else on last comment line
		           skipComment = ( (txtLine.indexOf(Constants.TEMPLATE_COMMENT_END) < 0 ) || 
		        		           (txtLine.indexOf(Constants.TEMPLATE_KEYWORD_END) < 0 ) );
		        } 
		        else {

			       if ( (txtLine.indexOf(Constants.TEMPLATE_COMMENT_START) >= 0) &&
			    		(txtLine.indexOf(Constants.TEMPLATE_KEYWORD_START) >= 0) ) {
			    	   skipComment = true;
		           }
			       else {
			    	   
				        // Allow for multiple variable substitutions within each line
						for(String templateVar : templateVarSet) {

							txtLine = txtLine.replace(templateVar, templateVarMap.get(templateVar));

						} // End template variable loop
						
						fileWriter.write(txtLine+'\n');

			       } // End if not skipping comments 
			       
		        } // End if not skipping comment block
		        
				
			} // End file read loop

			//System.out.println("Modified " + modifiedLines);				
			fileReader.close();
			fileWriter.close();

		} catch (IOException x) {
			
		    System.err.println(x);
		
		}
				
	} // End InstantiateFile()
	

	/*
	 * Modify the existing COSMOS cmd_tlm_server definition file.
	 * Return a success/failure status
	 */
   protected boolean UpdateCosmosCmdTlmServer(String appName) {
      
      String line = null;
      List<String> lines = new ArrayList<String>();

      String cosmosCmdTlmFile = targetCosmosConfigDir + Constants.COSMOS_CONFIG_TARGET_FILE;

      System.out.println("cosmosCmdTlmFile: " + cosmosCmdTlmFile);
      
      updatedCosmosCmdTlmFile = false;
      
      try {
      
         File cmdTlmFile = new File(cosmosCmdTlmFile);
         FileReader fr = new FileReader(cmdTlmFile);
         BufferedReader br = new BufferedReader(fr);
            
         System.out.println("********* ");
         // Simple logic with many assumptions
         //   Assumes user doesn't duplicate an existing target name
         //   Assume one set of start/end tags in proper order in the file
         // Logic won't duplicate new target if it exists between tags. 
         boolean betweenTags = false, addNewTarget = true;
         while ((line = br.readLine()) != null) {
            if (betweenTags && addNewTarget) {
               if (line.contains(appName.toUpperCase())) addNewTarget = false;
               if (line.contains(Constants.CFS_KIT_TAG_END)) {
                  // Only get here if still need to add new target and finished with cfs_kit block
                  lines.add("  TARGET " + appName.toUpperCase() + "\n");
                  addNewTarget = false;
               }
            } // End if between tags & need to add target
            else {
               betweenTags = line.contains(Constants.CFS_KIT_TAG_START);
            } // End if not between tags 
            lines.add(line+"\n");
         } // End while
         fr.close();
         br.close();

         FileWriter fw = new FileWriter(cmdTlmFile);
         BufferedWriter out = new BufferedWriter(fw);
         for(String s : lines)
            out.write(s);
         out.flush();
         out.close();
         
         updatedCosmosCmdTlmFile = true;
         
      } catch (Exception ex) {
         
         ex.printStackTrace();
         
         // May be situations when user can't update the COSMOS file so don't
         // treat it like an error and just retrun the status
         
      }
         
      return updatedCosmosCmdTlmFile;
            
   } // End UpdateCosmosCmdTlmServer()
     
	public String getTargetPath ()
	{
		return targetPath;
	
	} // End getTargetPath()
	
   public boolean updatedCosmosCmdTlmFile()
   {

      return updatedCosmosCmdTlmFile;
   
   } // End getTargetPath()
	
   /**
    *  
    * TODO - Replace with Junit.
    * 
    * @param args
    * @throws Exception
    *     
    */
   public static void main(String [] args) throws Exception
   {
	  // Template directory ends in separator and target directory doesn't
      String  templateDir = "C:\\Users\\dcmccom2\\Documents\\GitHub\\cfs-kit-vagrant\\cfs\\apps\\_template__app\\";
      String  targetDir = "C:\\Users\\dcmccom2\\Documents\\GitHub\\cfs-kit-vagrant\\cfs\\apps\\";
      //String  propertyFile = "C:\\dmccomas\\projects\\FSRL\\open\\tools\\cfs-appgen\\bin\\appgen-properties.xml";
      			
      File dir1 = new File (".");
      System.out.println ("Current dir : " + dir1.getCanonicalPath());
      System.out.println ("CWD: " + Constants.PATH_CWD);

      //CreateApp createApp = new CreateApp(Constants.PROP_FILE, templateDir, targetDir,"test");
         
   } // End main()

} // End class CreateApp
