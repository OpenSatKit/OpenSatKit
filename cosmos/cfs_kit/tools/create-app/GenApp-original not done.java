/*
 * 3/30/17 - I was cutting and pasting examples to build my app.  I started with an app that didn't inherited from JFrame
 * which the more common approach.  It didn't seem to be a problem until I wanted to use GroupLayout which needs
 * getContentPane(). Thsi is available from JFrame and not JPanel. I switched to another structure.
 */

import static javax.swing.GroupLayout.Alignment.BASELINE;
import static javax.swing.GroupLayout.Alignment.LEADING;

import java.awt.BorderLayout; 
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.net.URL;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileReader;

import javax.swing.GroupLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JEditorPane;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JTree;
import javax.swing.SwingConstants;
import javax.swing.UIManager;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;

public class GenApp extends JPanel {

    private JTree tree;
    private JTextArea preview;
    private JLabel status;
    private JEditorPane previewPane;
    
    private JButton templateButton, cfsButton, cosmosButton, exitButton;
    private JPanel  templatePanel, cfsPanel, cosmosPanel;
    private JLabel  templateLabel, cfsLabel, cosmosLabel;
    private JTextField  templateText, cfsText, cosmosText;
    
    // Optionally set the look and feel.
    private static boolean useSystemLookAndFeel = false;

    public GenApp(String directory) {
    	
    	super(new GridLayout(0,1));
    	
    	
    	//*******************************************************
        tree = new JTree(new FileSelectorModel(directory));

        preview = new JTextArea();
        preview.setWrapStyleWord(true);
        preview.setLineWrap(true);
        preview.setEditable(false);

        status = new JLabel(directory);

        tree.addTreeSelectionListener(new TreeSelectionListener() {

            public void valueChanged(TreeSelectionEvent e) {
                FileNode selectedNode = (FileNode) tree.getLastSelectedPathComponent();
                status.setText(selectedNode.getAbsolutePath());
                if (selectedNode.isFile()) {
                    preview.setText(null);
                    try {
                        BufferedReader br = new BufferedReader(new FileReader(selectedNode.getAbsolutePath()));
                        String line = "";
                        while ((line = br.readLine()) != null) {
                            preview.append(line);
                            preview.append(System.getProperty("line.separator"));
                        }
                    } catch (Exception exc) {
                        exc.printStackTrace();
                    }
                }
            }
        });

        //TODO - From TreeDemo
        //getContentPane().add(BorderLayout.WEST, new JScrollPane(tree));
        //getContentPane().add(BorderLayout.SOUTH, status);
        //getContentPane().add(new JScrollPane(preview));
        //Create the scroll pane and add the tree to it. 
        
        JScrollPane treeView = new JScrollPane(tree);

        //Create the HTML viewing pane.
        previewPane = new JEditorPane();
        previewPane.setEditable(false);
        JScrollPane previewView = new JScrollPane(previewPane);

        //Add the scroll panes to a split pane.
        JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        splitPane.setTopComponent(treeView);
        splitPane.setBottomComponent(previewView);

        Dimension minimumSize = new Dimension(100, 50);
        previewView.setMinimumSize(minimumSize);
        treeView.setMinimumSize(minimumSize);
        splitPane.setDividerLocation(100); 
        splitPane.setPreferredSize(new Dimension(500, 300));

    	//*******************************************************
        
        templatePanel = new JPanel();
        //templatePanel.setLayout(new GridLayout(2,2));

        templateText  = new JTextField(15);
        templateText.setText("TODO - Set default from prop file");
        templateLabel = new JLabel("Template Directory");
        templateButton = new JButton("...");
        //templateButton.addActionListener(this);
        templatePanel.add(templateLabel);
        templatePanel.add(templateButton);        
        templatePanel.add(templateText);

    	//*******************************************************
        
        cfsPanel = new JPanel();
        //cfsPanel.setLayout(new GridLayout(2,2));

        cfsText  = new JTextField(15);
        cfsText.setText("TODO - Set default from prop file");
        cfsLabel = new JLabel("Template Directory");
        cfsButton = new JButton("...");
        //cfsButton.addActionListener(this);
        cfsPanel.add(cfsLabel);
        cfsPanel.add(cfsButton);        
        cfsPanel.add(cfsText);

        //*******************************************************
        
        cosmosPanel = new JPanel();
        //cosmosPanel.setLayout(new GridLayout(2,2));

        cosmosText  = new JTextField(15);
        cosmosText.setText("TODO - Set default from prop file");
        cosmosLabel = new JLabel("Template Directory");
        cosmosButton = new JButton("...");
        //cosmosButton.addActionListener(this);
        cosmosPanel.add(cosmosLabel);
        cosmosPanel.add(cosmosButton);        
        cosmosPanel.add(cosmosText);
  

        //Add the split pane to this panel.
        add(templatePanel);
        add(splitPane);
        add(cfsPanel);
        add(cosmosPanel);
  
    } // End GenApp()
    
     
    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event dispatch thread.
     */
    private static void createAndShowGUI() {
        
    	if (useSystemLookAndFeel) {
            try {
                UIManager.setLookAndFeel(
                    UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                System.err.println("Couldn't use system look and feel.");
            }
        }

        //Create and set up the window.
        JFrame frame = new JFrame("TreeDemo");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //TODO - Add Property file read
        
        //Add content to the window.
        frame.add(new GenApp("C:\\"));

        //Display the window.
        frame.pack();
        frame.setSize(800, 600);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setTitle("cFS Application Generation");
        //frame.setIconImage(image);
        frame.setVisible(true);
        
    } // End createAndShowGUI()

    public static void main(String[] args) {
        // Schedule a job for the event dispatch thread:
        //  creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    } // End main()

} // End class GenApp
