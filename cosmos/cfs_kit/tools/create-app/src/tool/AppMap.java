package tool;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

/**
 * 
 * Utility class that maps information between the new app name
 * and the template variables. 
 * 
 * @author  David McComas
 * @license Licensed under the copyleft GNU General Public License (GPL).
 *
 */
public class AppMap {

   private String appName = "null";
   private String appNameArray[] = {"NULL", "Null", "null"}; // Case order is assumed        
   private Map<String, String> templateVarMap = new HashMap<String,String>(10);   // Template variable mappings   

   /**
    * 
    * @param appName    Name of new application in lower case
    * @param propFile   Name (directory and file name) of appgen property file
    */
   public AppMap(Properties templateProp, String appName) {
      

      setAppNameArray(appName);
        
      CreateTemplateVarMap(templateProp);
            
   } // End CreateApp()
   
   
   /**
    * Create an array of app names containing different lower/upper case permutations that will be 
    * used for different template variable string substitutions. 
    * 
    * Assumes a single word app name in terms of mixed case. 
    * 
    * @param appName  Name of new application.
    */
   protected void setAppNameArray(String appName) {
      
       this.appName = appName;
       
       this.appNameArray[0] = appName.toUpperCase();
       
       char[] appCharName = appName.toCharArray();
       appCharName[0] = appNameArray[0].charAt(0);
       this.appNameArray[1] = new String(appCharName);
       
       this.appNameArray[2] = this.appName;

       System.out.println("appNameArray: "+appNameArray[0]+", "+appNameArray[1]+", "+appNameArray[2]);
   
   } // End setAppNameArray()

   /*
   ** The property file defines the directories, files, and template variables
   ** that need to be created for a new app. This method creates a directory
   ** for the new app, loops through each sub-directory, and instantiates 
   ** each file for the new app. 
   */
   protected void CreateTemplateVarMap(Properties templateProp) {
   
      for(String key : templateProp.stringPropertyNames()) {
         if (key.startsWith(app.Constants.PROP_TEMPLATE_VAR_PREFIX)) {
            int i=0;
            String [] templateVars = templateProp.getProperty(key).split(",");
            for (String templateVar: templateVars) {
               templateVarMap.put(templateVar, appNameArray[i]);
               i++;
            } // End template variable loop
         } // End if var keyword
       } // End property loop
   
   } // End CreateTemplateVarMap()
   
   public String getAppName() {
      return appName;
   } // End getAppName()
   
   public String[] getAppNameArray() {
      return appNameArray;
   } // End getAppNameArray()
   
   public Map<String, String> getTemplateVarMap() {
      return templateVarMap;
   } // End getTemplateVarMap()
   
} // End class AppMap
