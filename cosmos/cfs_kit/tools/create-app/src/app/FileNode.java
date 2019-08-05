package app;
public class FileNode extends java.io.File {

    public FileNode(String directory) {
        super(directory);
    }

    public FileNode(FileNode parent, String child) {
        super(parent, child);
    }

    @Override
    public String toString() {
        return getName();
    }
}
