package tool;

import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;
import java.util.ArrayList;

import app.Constants;

/*
 * Manage the application templates
 */
public class AppTemplates {
   
   protected ArrayList<TemplateProp> templatePropList = new ArrayList<TemplateProp>();
   
   public AppTemplates(String templateBaseDirStr) {
	      
      boolean templatePropLoaded;          
	  
      File templateBaseDir = new File(templateBaseDirStr);

      System.out.println(">>> AppTemplates: " + templateBaseDirStr);
      
      if (templateBaseDir.isDirectory()) {
    	  
    	  //for (int i=0; i < templateDir.length; i++) {
          //if templateDir[i]. for(String key : templateProp.stringPropertyNames()) {

    	  for (File templateDir: templateBaseDir.listFiles()) {
    		        
    		  if (templateDir.isDirectory()) {
    			  
    			  /*  Check whether required template property file exists
    			  **  Not an error if it doesn't. Just means the directory won't be processed
    			  */ 
                 
    			  File templatePropFile = new File(templateDir.getAbsolutePath() + Constants.PATH_SEP + Constants.TMP_PROP_FILE);
    		  
                  System.out.println(">>> AppTemplates: templatePropFile for " + templateDir.getAbsolutePath() + Constants.PATH_SEP + Constants.TMP_PROP_FILE);

    			  if (templatePropFile.exists()) {
    			    	 
    			     System.out.println(">>> AppTemplates: templatePropFile exists");

    			     templatePropLoaded = false;

    			    	 try {
    			             
    			    	     Properties templateProp = new Properties();

    			             templateProp.loadFromXML(new FileInputStream(templatePropFile));

    			             templatePropList.add(new TemplateProp(templateProp, templateDir.getAbsolutePath()));
        			    	 
    			             templatePropLoaded = true;          
    			         
    			             System.out.println("Created template property object for " + templatePropFile.getAbsolutePath());

    			    	 } catch (Exception e) {

    			             if (templatePropLoaded) {
    			                System.out.println("Error creating template property object from file " + templatePropFile.getAbsolutePath() + Constants.PATH_SEP + templatePropFile.getName());
    			             }
    			             else {
    			                System.out.println("Error loading/processing template property file " + templatePropFile.getAbsolutePath() + Constants.PATH_SEP + templatePropFile.getName());
    			             }

    			          } // End property file exception catch 

    			     } // End if template property file exists
    			  
    		  
    		  } // End if template directory
    		  
    	  } // End template dir file loop
    	  
      } // End if template base directory exists
      else {
   
    	  // TODO -error message
    	  
      } // End if template base directory does not exist
      	            
   } // End CreateApp()
	   

   public String[] getTitles() {
	
	  ArrayList<String> titles = new ArrayList<String>();
	   
      for (TemplateProp template : templatePropList) {
         titles.add(template.getTitle());
      }
	   
      return titles.toArray(new String[0]);
	   
   } // End getTitles()

   public TemplateProp getTemplate(int i) {

	   TemplateProp template = null;
	   
	   if (i < templatePropList.size())
		   template = templatePropList.get(i);
		   
	   return template;
	   
   } // End getTemplate()
   
} // End AppTemplates
