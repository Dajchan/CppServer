//
//  FileBrowserViewController.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 06.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa


extension NSNotification.Name {
    public static let FileSelectionDidChange: NSNotification.Name = NSNotification.Name(rawValue: "FileSelectionDidChange")
}

class FileBrowserViewController: NSViewController, NSOutlineViewDelegate {
    
    dynamic var nodes = [Node]()
    
    @IBOutlet weak var outlineView: NSOutlineView!
    @IBOutlet weak var addPullDownButton: NSPopUpButton!
    
    var rootPath: String = ""
    
    
    func initNodes() {
        let node = Node(path: "Node", type: Node.FileType.Undefined)
        let group = Node(path: "Group", type: Node.FileType.Undefined)
        let nodeA = Node(path: "Node A", type: Node.FileType.Undefined)
        let nodeB = Node(path: "Node B", type: Node.FileType.Undefined)
        group.children = [nodeA, nodeB]
        nodes = [node, group]
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.outlineView.delegate = self
        initNodes()
        NotificationCenter.default.addObserver(self, selector: #selector(FileBrowserViewController.reloadRootFromNotification), name: NSNotification.Name.SessionDidChange, object: nil)
    }
    
    func reloadRootFromNotification(notification: Notification) {
        print("reloadRootFromNotification")
        if (notification.object != nil) {
            let session = notification.object as! Session
            self.rootPath = session.rootFilePath + "/website"
            reloadRoot()
        }
    }
    
    func reloadRoot() {
        print("reloadRoot from: " + self.rootPath)
        let rootNodes = collectNodes(directoryOrFile: self.rootPath)
        if rootNodes != nil {
            nodes = rootNodes!.children
            for n in nodes {
                n.canEdit = false
            }
        } else {
            nodes = [Node]()
        }
        evalPullDownOptions()
    }
    
    func collectNodes(directoryOrFile: String) -> Node? {
        var isDir: ObjCBool = false
        if FileManager.default.fileExists(atPath: directoryOrFile, isDirectory: &isDir) {
            let node = Node(path: directoryOrFile, type: (isDir.boolValue ? Node.FileType.Dir : Node.FileType.File))
            if isDir.boolValue {
                let s: NSString = directoryOrFile as NSString
                let filesOrDirs = try! FileManager.default.contentsOfDirectory(atPath: directoryOrFile)
                var subNodes = [Node]()
                for d in filesOrDirs {
                    if !d.hasPrefix(".") {
                        let subNode = collectNodes(directoryOrFile: s.appendingPathComponent(d))
                        if subNode != nil {
                            subNodes.append(subNode!)
                        }
                    }
                }
                node.children = subNodes
                // set as dir
                
            } else {
                // set as file
            }
            return node
        }
        return nil
    }
    
    func evalPullDownOptions() {
        let item = NSMenuItem(title: "", action: nil, keyEquivalent: "")
        item.image = NSImage(named: NSImageNameAddTemplate)
        
        let menu = NSMenu(title: "")
        menu.addItem(item)
        menu.addItem(NSMenuItem(title: "a", action: nil, keyEquivalent: "a"))
        menu.addItem(NSMenuItem(title: "b", action: nil, keyEquivalent: "b"))
        self.addPullDownButton.menu = menu
    }
    
    @IBAction func editNode(sender: NSObject) {
        let row = outlineView.selectedRow
        if row != -1 {
            outlineView.editColumn(0, row: row, with: nil, select: true)
        }
    }
    
    @IBAction func addNode(sender: NSObject) {
        
        print("addNode")
    }
    
    
// MARK: NSOutlineViewDelegate

    func outlineViewSelectionDidChange(_ notification: Notification) {
        print(self.outlineView.selectedRow)
    }
    
    func outlineView(_ outlineView: NSOutlineView, shouldEdit tableColumn: NSTableColumn?, item: Any) -> Bool {
        print(item)
        return true
    }

}
