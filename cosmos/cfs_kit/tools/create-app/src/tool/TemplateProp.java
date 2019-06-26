package tool;

import java.util.Enumeration;
import java.util.Properties;

import app.Constants;

public class TemplateProp {

   private String title, version, descr;
   private String absDir;
   
   private boolean hasCfsTemplate, hasCosmosTemplate;
   
   public TemplateProp (Properties templateProp, String absTemplateDir) {
	   
	  absDir = absTemplateDir;
	  
      title   = templateProp.getProperty(Constants.PROP_TEMPLATE_TITLE);
      if (title == null) title = Constants.UNDEF_STR;
      
      version = templateProp.getProperty(Constants.PROP_TEMPLATE_VERSION);
      if (version == null) version = Constants.UNDEF_STR;

      descr   = templateProp.getProperty(Constants.PROP_TEMPLATE_DESCR);
      if (descr == null) descr = Constants.UNDEF_STR;
	   
      
      hasCfsTemplate    = false;
      hasCosmosTemplate = false;

      Enumeration e = templateProp.propertyNames();
      while (e.hasMoreElements()) {
    	  
    	  String key = (String) e.nextElement();
    	  
    	  if (key.contains(Constants.PROP_TEMPLATE_CFS_APP_DIR_PREFIX)) {
    		  hasCfsTemplate = true;
			  System.out.println(">>> TemplateProp: Found cFS dir in key " + key);
    	  }
    	  else if (key.contains(Constants.PROP_TEMPLATE_COSMOS_DIR_PREFIX)) {
    		  hasCosmosTemplate = true;
			  System.out.println(">>> TemplateProp: Found COSMOS dir in key " + key);
    	  }
    	  
      } // End property name loop
	  	 
	  System.out.println(">>> TemplateProp: Created template for " + title + ", " + version + ", " + descr);  
      
   } // End TemplateProp
	
   public String getAbsDir()  { return absDir;   }

   public String getTitle()   { return title;   }
   public String getVersion() { return version; }
   public String getDescr()   { return descr;   }
   
   public boolean hasCfs()    { return hasCfsTemplate;    }
   public boolean hasCosmos() { return hasCosmosTemplate; }
   
   
} // End class TemplateProp
