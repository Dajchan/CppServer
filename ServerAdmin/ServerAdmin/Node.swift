//
//  Node.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 12.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa



class Node: NSObject {
    public enum FileType : Int {
        case Dir
        case File
        case Undefined
    }
    
    var title = "Node" {
        didSet(oldTitle) {
            let p = nsPath()
            let oldPath = p.appendingPathComponent(oldTitle)
            let newPath = p.appendingPathComponent(title)
            
            if oldTitle != title && FileManager.default.fileExists(atPath: oldPath) && !FileManager.default.fileExists(atPath: newPath) {
                try! FileManager.default.moveItem(atPath: oldPath, toPath: newPath)
            }
        }
    }
    var path = ""
    var type = Node.FileType.Undefined
    var children = [Node]()
    var canEdit = true
    
    override init() {
        super.init()
    }
    
    init(path: String, type: Node.FileType) {
        self.title = (path as NSString).lastPathComponent
        self.path = (path as NSString).deletingLastPathComponent
        self.type = type
    }
    
    private func nsPath() -> NSString {
        return path as NSString
    }
}
