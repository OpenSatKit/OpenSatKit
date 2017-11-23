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
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;

import static javax.swing.GroupLayout.Alignment.*;

import tool.AppMap;
import tool.CreateApp;

public class GenApp extends JFrame {

   private Properties appGenProp = new Properties();
	private String errStr;
	
	private JFrame appFrame;
	
   private JTree tree;
   private JTextArea preview;
   private JEditorPane previewPane;
   
   private String  templateDirStr, cfsDirStr, cosmosDirStr;
   private JPanel  templatePanel, cfsPanel, cosmosPanel;
   private JLabel  templateLabel, cfsLabel, cosmosLabel;
   private JButton templateButton, cfsButton, cosmosButton, createButton;
   private JTextField  templateText, cfsText, cosmosText;

   private JFileChooser chooser;
    
   public GenApp() {
	
      appFrame = (JFrame) SwingUtilities.getRoot(this);
      
      configProperties();   // Must be called prior to GUI setup

      //////////// Create source panel: Instructions and template directory
      JPanel sourcePanel = new JPanel();
      sourcePanel.setLayout(new GridLayout(2,0));
      
      JPanel instructPanel = new JPanel();
      createButton = new JButton("Create App");
      addInstructPane(instructPanel, createButton );

      templatePanel  = new JPanel();
      templateLabel  = new JLabel(" Template Dir:");;
      templateText   = new JTextField(templateDirStr);
      templateButton = new JButton("Browse ...");
      addDirPane(templatePanel, templateLabel, templateText, templateButton);

      sourcePanel.add(instructPanel);
      sourcePanel.add(templatePanel);
      getContentPane().add(BorderLayout.PAGE_START,sourcePanel);

      //////////// Create template directory panel

      tree = new JTree(new FileSelectorModel(templateDirStr));

      preview = new JTextArea();
      preview.setWrapStyleWord(true);
      preview.setLineWrap(true);
      preview.setEditable(false);

      
      //////////// Create tree & split directory scroll pane

      tree.addTreeSelectionListener(new TreeSelectionListener() {

         public void valueChanged(TreeSelectionEvent e) {
                
            createButton.setEnabled(false);
            preview.setText("\n\n\n   " + app.Constants.TEMPLATE_PROP_FILE +
                  " doesn't exist in the current directory.");
            FileNode selectedNode = (FileNode) tree.getLastSelectedPathComponent();
            if (selectedNode.isDirectory()) {
               templateText.setText(selectedNode.getAbsolutePath());
               String templatePropFileStr = selectedNode.getAbsolutePath() + app.Constants.PATH_SEP + app.Constants.TEMPLATE_PROP_FILE;
               File templatePropFile = new File(templatePropFileStr);
               if (templatePropFile.exists()) {
                  createButton.setEnabled(true);
                  try {
                     BufferedReader br = new BufferedReader(new FileReader(templatePropFileStr));
                     String line = "";
                     while ((line = br.readLine()) != null) {
                        preview.append(line);
                        preview.append(System.getProperty("line.separator"));
                     }
                     br.close();
                  } catch (Exception exc) {
                     exc.printStackTrace();
                  }
               } // End if template property file exists
            } // End if selected a directory
         
         } // End valueChanged()
         });

      JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);       //Add the scroll panes to a split pane.
      splitPane.setTopComponent(tree);
      splitPane.setBottomComponent(preview);
      //TODO - Decide vertical vs horizontal getContentPane().add(BorderLayout.CENTER, splitPane);
        
      getContentPane().add(BorderLayout.LINE_START, new JScrollPane(tree));
      getContentPane().add(new JScrollPane(preview));

      //////////// Create composite target directory panel
      //getContentPane().add(BorderLayout.PAGE_END, status);
      JPanel targetPanel = new JPanel();
      targetPanel.setLayout(new GridLayout(2,0));

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
      getContentPane().add(BorderLayout.PAGE_END, targetPanel);
        
      createButtonListeners();  
         
      createButton.setEnabled(false);
      pack();
      setTitle("cFS Application Generation");
      setSize(800, 600);
      setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      setVisible(true);

   } // End GenApp()
	
   private void configProperties() {
   
      try {
      
         appGenProp.loadFromXML(new FileInputStream(app.Constants.APP_PROP_FILE));
         
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
         
         System.out.println("Property file returned null for " + dirPropKey + " directory. Using " + dirDefaultStr);
         returnDirStr = dirDefaultStr;
      }
      else if (returnDirStr.length() < 1) {
         
         System.out.println("Property file string length of zero for " + dirPropKey + " directory. Using " +  dirDefaultStr);
         returnDirStr = dirDefaultStr;
      }
      else
      {
         System.out.println("Property file supplied  " + dirPropKey + " directory " + returnDirStr);
         returnDirStr = Constants.PATH_CWD + Constants.PATH_SEP + returnDirStr;
      }
   
      return returnDirStr;
      
   } // End getDefaultDirStr()
   
   
   private void addInstructPane(Container panel, JButton button) {

      panel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();

      JLabel instructLabel = new JLabel(
      "Select a template directory using window tree or browse button. " + app.Constants.TEMPLATE_PROP_FILE + 
      " will be displayed when available.");
      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 0.9;
      c.gridx = 0;
      c.gridy = 0;
      panel.add(instructLabel, c);

      c.fill = GridBagConstraints.HORIZONTAL;
      c.weightx = 0.1;
      c.gridx = 2;
      c.gridy = 0;
      panel.add(button, c);
      
   } // End addInstructPane()
   
   private void addDirPane(Container panel, JLabel label, JTextField textField, JButton button) {

      panel.setLayout(new GridBagLayout());
      GridBagConstraints c = new GridBagConstraints();

      c.fill = GridBagConstraints.HORIZONTAL;
	   c.weightx = 0.1;
	   c.gridx = 0;
	   c.gridy = 0;
	   panel.add(label, c);

	   c.fill = GridBagConstraints.HORIZONTAL;
	   c.weightx = 0.8;
      c.gridx = 1;
      c.gridy = 0;
      panel.add(textField, c);

    	c.fill = GridBagConstraints.HORIZONTAL;
    	c.weightx = 0.1;
    	c.gridx = 2;
    	c.gridy = 0;
    	panel.add(button, c);
    	
   } // End addDirPane()

    
   private void createButtonListeners() {  
   
      createButton.addActionListener(new ActionListener() {
         
         public void actionPerformed(ActionEvent e) {
         
            File cfsDest    = new File(cfsText.getText());
            File cosmosDest = new File(cosmosText.getText());
            if (cfsDest.isDirectory() && cosmosDest.isDirectory() ) {
               System.out.println("Create App: templatetext:"+templateText.getText());
               createApp(templateText.getText(), cfsDest.getAbsolutePath(), cosmosDest.getAbsolutePath());
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

      templateButton.addActionListener(new ActionListener() {
      
         public void actionPerformed(ActionEvent e) {
         
            chooser = new JFileChooser(); 
            chooser.setCurrentDirectory(new java.io.File(templateText.getText()));
            chooser.setDialogTitle("Select Template Directory");
            chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            chooser.setAcceptAllFileFilterUsed(false); // disable the "All files" option.
             
            Component component = (Component) e.getSource();
            JFrame frame = (JFrame) SwingUtilities.getRoot(component);
            if (chooser.showOpenDialog(frame) == JFileChooser.APPROVE_OPTION) { 
               System.out.println("getCurrentDirectory(): " + chooser.getCurrentDirectory());
               System.out.println("getSelectedFile() : " + chooser.getSelectedFile());
               templateText.setText(chooser.getCurrentDirectory().getAbsolutePath());
            }
            else {
               System.out.println("No Selection ");
            }
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
               cfsText.setText(chooser.getCurrentDirectory().getAbsolutePath());
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
               cosmosText.setText(chooser.getCurrentDirectory().getAbsolutePath());
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
        
         String templateFileStr =  srcTemplateDirStr+Constants.PATH_SEP+app.Constants.TEMPLATE_PROP_FILE;

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


