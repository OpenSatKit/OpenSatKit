package app;
/**
 * This application provides tools to generate a new cFS application using  
 * using the template files defined in the source template directory tree.
 * It's really a general purpose tool that instantiates template files
 * from the source directory tree to the destination tree. The appgen 
 * property file identifies the subdirectories and files to be instantiated.
 * 
 * TODO - Need precise error reporting. User's need to know exactly what failed
 *        in their property definition file
 * 
 * @author David McComas
 * @license Licensed under the copyleft GNU General Public License (GPL).
 */

import java.util.Properties;  
import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import tool.AppMap;
import tool.AppTemplates;
import tool.CreateApp;

public class GenApp extends JFrame {

   private boolean    propFileValid = false;
   private Properties appGenProp = new Properties();
   private String errStr = Constants.UNDEF_STR;

   private int selectedTemplate=0;
   private AppTemplates appTemplates;
	
   private JFrame appFrame;
   
   // Template 
   private String templateDirStr;
   private JLabel templateLabel;
   private JComboBox<String> templateCombo;
   private JTextField templateVerText;
   private JTextField templateDescrText;
   private JCheckBox templateGenCfs;
   private JCheckBox templateGenCosmos;
   
   // cfS & COSMOS destination directories have same UI
   private String       cfsDirStr, cosmosDirStr;
   private JPanel       cfsPanel,  cosmosPanel;
   private JLabel       cfsLabel,  cosmosLabel;
   private JButton      cfsButton, cosmosButton;
   private JTextField   cfsText,   cosmosText;

   // User action buttons
   private JButton createButton;
   private JButton installButton;

   private JFileChooser chooser;
    
   public GenApp() {
	   
      appFrame = (JFrame) SwingUtilities.getRoot(this);
      
      // configProperties() must be called first and
      // both functions must be called prior to GUI setup
      configProperties();
	  appTemplates = new AppTemplates(templateDirStr);

      // Create panels

	  JPanel templatePanel = createTemplatePanel();
      JPanel instructPanel = createInstructPanel();
      JPanel buttonPanel   = createButtonPanel();
      JPanel targetPanel   = createTargetPanel();
      
      getContentPane().add(BorderLayout.NORTH,templatePanel);
      getContentPane().add(BorderLayout.WEST,instructPanel);
      getContentPane().add(BorderLayout.EAST,buttonPanel);
      getContentPane().add(BorderLayout.PAGE_END, targetPanel);
      
      createButtonListeners();  
         
      //**createButton.setEnabled(false);
      pack();
      setTitle("cFS Application Generation");
      setSize(600, 300);
      setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      setVisible(true);

      if (!propFileValid) {
         JOptionPane.showMessageDialog(new JFrame(), errStr, "Property File Error", JOptionPane.ERROR_MESSAGE);
      }
       

   } // End GenApp()
	
   private void configProperties() {
   
      try {
      
         appGenProp.loadFromXML(new FileInputStream(app.Constants.APP_PROP_FILE));
         propFileValid = true;
         
      } catch (Exception e) {

         System.out.println("Couldn't open AppGen property file at " + app.Constants.APP_PROP_FILE);
         errStr = "Couldn't open AppGen property file at " + app.Constants.APP_PROP_FILE;
          
      } // End property file exception catch 

      String iconDirFile = Constants.IMAGE_DIR + Constants.CFS_ICON_FILE;
      try {
         
         setIconImage(new ImageIcon(iconDirFile).getImage());
        
      } catch (Exception e) {

         // Just do without the icon
         System.out.println("Couldn't open icon file at " + iconDirFile);

      }
      
      templateDirStr = getDefaultDirStr(app.Constants.PROP_APP_TEMPLATE_DIR,      Constants.TEMPLATE_DIR);
      cfsDirStr      = getDefaultDirStr(app.Constants.PROP_APP_TARGET_CFS_DIR,    Constants.TARGET_CFS_DIR);
      cosmosDirStr   = getDefaultDirStr(app.Constants.PROP_APP_TARGET_COSMOS_DIR, Constants.TARGET_COSMOS_DIR);
      
   } // End configProperties()

   private String  getDefaultDirStr(String dirPropKey, String dirDefaultStr) {
   
      String returnDirStr = appGenProp.getProperty(dirPropKey);
      
      if (returnDirStr == null) {
         
         System.out.println("Property file returned null for " + dirPropKey + " directory. Using default directory " + dirDefaultStr);
         returnDirStr = dirDefaultStr;
      }
      else if (returnDirStr.length() < 1) {
         
         System.out.println("Property file string length of zero for " + dirPropKey + " directory. Using default directory " +  dirDefaultStr);
         returnDirStr = dirDefaultStr;
      }
      else
      {
         System.out.println("Property file key " + dirPropKey + " supplied directory " + returnDirStr);
         if (returnDirStr.startsWith(Constants.PATH_SEP) || returnDirStr.contains(":")) {
            
            String  fileStr = new File(returnDirStr).getAbsolutePath();
            System.out.println("**FILE string (user supplied abs path) = " + fileStr);
            // No need to create returnDirStr because prop file supplied the full path            
         
         } // If property starts with 
         else {
         
            String  fileStr = new File(returnDirStr).getAbsolutePath();
            System.out.println("**FILE string (user supplied rel path) = " + fileStr);
            returnDirStr = Constants.PATH_CWD + Constants.PATH_SEP + returnDirStr;
         
         }
         
      } // End if returned valid property file value
   
      return returnDirStr;
      
   } // End getDefaultDirStr()
   
   
   private JPanel createInstructPanel() {

	  JPanel instructPanel = new JPanel();
      instructPanel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();

      JTextArea instructTextArea = new JTextArea(5, 32);
      instructTextArea.setEditable(false);
      instructTextArea.append("\n  1. Select a template from the drop down menu\n");
      instructTextArea.append("  2. Select the cFS and COSMOS destination directories\n");
      instructTextArea.append("  3. Click <Create  App> to generate the code\n");
      instructTextArea.append("  4. Click <Install App> to install the app into the kit\n");
      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 0.9;
      c.gridx = 0;
      c.gridy = 0;
      instructPanel.add(instructTextArea, c);

      return instructPanel;
    		  
   } // End createInstructPanel()

   private JPanel createTemplatePanel() {

      JPanel templatePanel = new JPanel();
      templatePanel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();
      
	  // Line 1: Template Selection, Version
      
      templateLabel = new JLabel("   Template: ");
	  templateCombo = new JComboBox<String>(appTemplates.getTitles());
      templateCombo.setSelectedIndex(selectedTemplate);

      JLabel templateVerLabel = new JLabel("   Version: ");
	  templateVerText = new JTextField(appTemplates.getTemplate(selectedTemplate).getVersion());

      // Line 2: What will be generated: cFS & COSMOS 

	  templateGenCfs= new JCheckBox("Generates cFS App");
      templateGenCfs.setSelected(appTemplates.getTemplate(selectedTemplate).hasCfs());
	  
      templateGenCosmos= new JCheckBox("Generates COSMOS cmd & tlm files");
      templateGenCosmos.setSelected(appTemplates.getTemplate(selectedTemplate).hasCosmos());
	  
	  // Line 3: Description
	  
	  templateDescrText = new JTextField(appTemplates.getTemplate(selectedTemplate).getDescr());
	  templateDescrText.setEditable(false);

      c.fill = GridBagConstraints.HORIZONTAL;
     
      c.weightx = 0; c.gridx = 0; c.gridy = 0;
      templatePanel.add(templateLabel, c);
      
      c.weightx = 1; c.gridx = 1; c.gridy = 0;
      templatePanel.add(templateCombo, c);

      c.weightx = 0; c.gridx = 2; c.gridy = 0;
      templatePanel.add(templateVerLabel, c);

      c.weightx = 1; c.gridx = 3; c.gridy = 0;
      templatePanel.add(templateVerText, c);
      
      c.weightx = 0; c.gridx = 0; c.gridy = 1;
      templatePanel.add(templateGenCfs, c);

      c.weightx = 0; c.gridx = 1; c.gridy = 1;
      templatePanel.add(templateGenCosmos, c);

      c.gridwidth = 2;
      c.weightx = 0; c.gridx = 0; c.gridy = 2;
      templatePanel.add(templateDescrText, c);

      return templatePanel;
    		  
   } // End createTemplatePanel()

   private JPanel createButtonPanel() {

	  JLabel blankLabel = new JLabel("   ");
      JPanel buttonPanel = new JPanel();
	  //**buttonPanel.setLayout(new GridLayout(0,4));
	  buttonPanel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();
	  c.fill = GridBagConstraints.HORIZONTAL;
	  c.weightx = 1; 
      
	  createButton  = new JButton("Create App");
	  installButton = new JButton("Install App");
	   	     
      c.gridx = 0; c.gridy = 0;
	  buttonPanel.add(createButton);

	  c.gridx = 0; c.gridy = 1;
	  buttonPanel.add(blankLabel);

      c.gridx = 0; c.gridy = 2;
	  buttonPanel.add(installButton);

	  return buttonPanel;
	      
   } // End createButtonPanel()

   
   private JPanel createTargetPanel() {

      JPanel targetPanel = new JPanel();
      targetPanel.setLayout(new GridLayout(3,0));
   
      cfsPanel  = new JPanel();
      cfsLabel  = new JLabel(" cFS app Dir:               ");;
      cfsText   = new JTextField(cfsDirStr);
      cfsButton = new JButton("Browse ...");
      addDirPane(cfsPanel, cfsLabel, cfsText, cfsButton );
     
      cosmosPanel  = new JPanel();
      cosmosLabel  = new JLabel(" COSMOS config Dir:");;
      cosmosText   = new JTextField(cosmosDirStr);
      cosmosButton = new JButton("Browse ...");
      addDirPane(cosmosPanel, cosmosLabel, cosmosText, cosmosButton);
     
      targetPanel.add(cfsPanel);
      targetPanel.add(cosmosPanel);

      return targetPanel;
      
   } // End createTargetPanel()

   private void addDirPane(Container panel, JLabel label, JTextField textField, JButton button) {

      panel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();

      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 0;
      c.gridx = 0;
      c.gridy = 0;
      panel.add(label, c);

      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 1;
      c.gridx = 1;
      c.gridy = 0;
      panel.add(textField, c);

      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 0;
      c.gridx = 2;
      c.gridy = 0;
      panel.add(button, c);
    	
   } // End addDirPane()

    
   private void createButtonListeners() {  
   
      templateCombo.addActionListener(new ActionListener() {
      
         public void actionPerformed(ActionEvent e) {
         
         	selectedTemplate = templateCombo.getSelectedIndex();
            System.out.println("Combo selection = " + selectedTemplate); 
            templateVerText.setText(appTemplates.getTemplate(selectedTemplate).getVersion());
            templateGenCfs.setSelected(appTemplates.getTemplate(selectedTemplate).hasCfs());
            templateGenCosmos.setSelected(appTemplates.getTemplate(selectedTemplate).hasCosmos());
            templateDescrText.setText(appTemplates.getTemplate(selectedTemplate).getDescr());
            System.out.println(appTemplates.getTemplate(selectedTemplate).getVersion() + ", " +appTemplates.getTemplate(selectedTemplate).getDescr());
         } // End actionPerformed()
      });
 
      createButton.addActionListener(new ActionListener() {
          
         public void actionPerformed(ActionEvent e) {
          
            File cfsDest    = new File(cfsText.getText());
            File cosmosDest = new File(cosmosText.getText());
            if (cfsDest.isDirectory() && cosmosDest.isDirectory() ) {
               System.out.println("Create App using template directory: " + appTemplates.getTemplate(selectedTemplate).getAbsDir());
               createApp(appTemplates.getTemplate(selectedTemplate).getAbsDir(), cfsDest.getAbsolutePath(), cosmosDest.getAbsolutePath());
            } 
            else {
               Component component = (Component) e.getSource();
               JFrame frame = (JFrame) SwingUtilities.getRoot(component);
               JOptionPane.showMessageDialog(frame,
                  "cFS and COSMOS targets must be directories",
                  "Target Directory Error",
                  JOptionPane.ERROR_MESSAGE);
            }
         } // End actionPerformed()
      });

      installButton.addActionListener(new ActionListener() {
          
         public void actionPerformed(ActionEvent e) {
        	
            Component component = (Component) e.getSource();
            JFrame frame = (JFrame) SwingUtilities.getRoot(component);
            JOptionPane.showMessageDialog(frame,
                      "The install app feature has not been implemented.",
                      "Coming soon...",
                      JOptionPane.INFORMATION_MESSAGE);

           
          } // End actionPerformed()
       });
      cfsButton.addActionListener(new ActionListener() {
         
         public void actionPerformed(ActionEvent e) {
         
            chooser = new JFileChooser(); 
            chooser.setCurrentDirectory(new java.io.File(cfsText.getText()));
            chooser.setDialogTitle("Select cFS App  Directory");
            chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            chooser.setAcceptAllFileFilterUsed(false); // disable the "All files" option.
             
            Component component = (Component) e.getSource();
            JFrame frame = (JFrame) SwingUtilities.getRoot(component);
            if (chooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) { 
               System.out.println("getCurrentDirectory(): " + chooser.getCurrentDirectory());
               System.out.println("getSelectedFile() : " + chooser.getSelectedFile());
               cfsText.setText(chooser.getSelectedFile().getAbsolutePath());
            }
            else {
               System.out.println("No Selection ");
            }
         } // End actionPerformed()
      });

      cosmosButton.addActionListener(new ActionListener() {
         
         public void actionPerformed(ActionEvent e) {
         
            chooser = new JFileChooser(); 
            chooser.setCurrentDirectory(new java.io.File(cosmosText.getText()));
            chooser.setDialogTitle("Select COSMOS config Directory");
            chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            chooser.setAcceptAllFileFilterUsed(false); // disable the "All files" option.
             
            Component component = (Component) e.getSource();
            JFrame frame = (JFrame) SwingUtilities.getRoot(component);
            if (chooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) { 
               System.out.println("getCurrentDirectory(): " + chooser.getCurrentDirectory());
               System.out.println("getSelectedFile() : " + chooser.getSelectedFile());
               cosmosText.setText(chooser.getSelectedFile().getAbsolutePath());
            }
            else {
               System.out.println("No Selection ");
            }
         } // End actionPerformed()
      });

   } // End createButtonListeners()

   /**
    * Manage the create of a new application
    * 
    */
   private boolean createApp(String srcTemplateDirStr, String destCfsDirStr, String destCosmosDirStr) {
      
      boolean appCreated = false;
      boolean templatePropLoaded = false;
      Properties templateProp = new Properties();
      
      String appName = JOptionPane.showInputDialog(appFrame, "Enter name of new app");
      if (appName != null) {
        
         String lowAppName = appName.toLowerCase();
        
         String templateFileStr =  srcTemplateDirStr+Constants.PATH_SEP+app.Constants.TMP_PROP_FILE;

         try {
           
            AppMap appMap = new AppMap (appGenProp, lowAppName);

            templateProp.loadFromXML(new FileInputStream(templateFileStr));
            templatePropLoaded = true;          
            System.out.println("Loaded property file " + templateFileStr);

            CreateApp app = new CreateApp(appMap, srcTemplateDirStr+Constants.PATH_SEP, destCfsDirStr+Constants.PATH_SEP, destCosmosDirStr+Constants.PATH_SEP);
            app.ProcessTemplateFile(templateProp, lowAppName);
            
            JOptionPane.showMessageDialog(appFrame,
                  "Created " + appName + " in " + cfsDirStr,
                  "Application Created",
                  JOptionPane.INFORMATION_MESSAGE);
            
            String instructions = "\n" +
                  "To complete the cFS application installation perform the following steps:\n\n" +
                  "  1. In the cfs/sample_defs directory update the following files:\n" +
                  "     - targets.cmake: Add the new app to the line with SET(TGT1_APPLIST ...)\n" +
                  "     - cpu1_cfe_es_startup.scr: Add a new app entry\n\n" +
                  "  2. Use cmake to build the new cFS image\n\n";            
            
            appCreated = true;
            
            if (app.updatedCosmosCmdTlmFile()) {
               instructions = instructions + 
                  "The COSMOS cmd_tlm_server file " + Constants.COSMOS_CONFIG_TARGET_FILE +"\n" +
                  "was successfully updated. Restart the cmd_tlm_server for the new app to be installed.\n";
            }
            else {
               instructions = instructions + 
                  "The COSMOS cmd_tlm_server file " + Constants.COSMOS_CONFIG_TARGET_FILE +"\n" +
                  "was not updated. You must manually add the new app's target to the fille and\n" +
                  "restart the cmd_tlm_server for the new app to be installed.";
            }

            //ScrollTextDialog instructDialog = new ScrollTextDialog(getShell());
            //instructDialog.open(instructions);
           
         } catch (Exception e) {

            if (templatePropLoaded) {
               System.out.println("Error creating app using template file " + templateFileStr);
               JOptionPane.showMessageDialog(appFrame,
                     "Error creating app using template file  " + templateFileStr,
                     "Application Creation Error",
                     JOptionPane.INFORMATION_MESSAGE);
            }
            else {
               System.out.println("Error loading/processing template property file " + templateFileStr);
               JOptionPane.showMessageDialog(appFrame,
                     "Error loading/processing template property file " + templateFileStr,
                     "Application Creation Error",
                     JOptionPane.INFORMATION_MESSAGE);
            }

         } // End property file exception catch 

      } // End if dialog returned true
   
      return appCreated;
     
   } // End createApp()

   public static void main(String args[]) {
        
	   java.awt.EventQueue.invokeLater(new Runnable() {
         public void run() {
            try {
               UIManager.setLookAndFeel(
                   "javax.swing.plaf.metal.MetalLookAndFeel");
                   //  "com.sun.java.swing.plaf.motif.MotifLookAndFeel");
                   //UIManager.getCrossPlatformLookAndFeelClassName());
            } catch (Exception ex) {
                ex.printStackTrace();
            }
             new GenApp().setVisible(true);
            }
       });
    
	} // End main()
    
} // End class GenApp

