//
//  SessionSetupAlert.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 07.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa

class SessionSetupAlert: NSAlert {
    
    let localPath: NSTextField
    let localURL: NSTextField
    let localPort: NSTextField
    
    let remoteURL: NSTextField
    let remoteUser: NSTextField
    let remotePassword: NSTextField
    
    
    override init() {
        localPath = NSTextField()
        localPath.placeholderString = "Local Path"
        localPath.stringValue = "/Users/skar/Desktop/server"
        
        localPort = NSTextField()
        localPort.placeholderString = "Local Port"
        localPort.stringValue = "3001"
        
        localURL = NSTextField()
        localURL.placeholderString = "Local URL"
        localURL.stringValue = "http://localhost:3001"
        
        remoteURL = NSTextField()
        remoteURL.placeholderString = "Remote URL"
        remoteURL.stringValue = "http://www.spiegel.de"
        
        remoteUser = NSTextField()
        remoteUser.placeholderString = "Remote User"
        remoteUser.stringValue = "u"
        
        remotePassword = NSSecureTextField()
        remotePassword.placeholderString = "Remote Password"
        remotePassword.stringValue = "p"
        
        
        localPath.translatesAutoresizingMaskIntoConstraints = false
        localURL.translatesAutoresizingMaskIntoConstraints = false
        
        remoteURL.translatesAutoresizingMaskIntoConstraints = false
        remoteUser.translatesAutoresizingMaskIntoConstraints = false
        remotePassword.translatesAutoresizingMaskIntoConstraints = false
        
        super.init()
        
        let containerView = NSView()
        containerView.translatesAutoresizingMaskIntoConstraints = false
        
        containerView.addSubview(localPath)
        containerView.addSubview(localURL)
        
        containerView.addSubview(remoteURL)
        containerView.addSubview(remoteUser)
        containerView.addSubview(remotePassword)
        
        var last: NSView?
        
        for v in [localPath, localURL, remoteURL, remoteUser, remotePassword] {
            v.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.width, relatedBy: NSLayoutRelation.greaterThanOrEqual, toItem: nil, attribute: NSLayoutAttribute.notAnAttribute, multiplier: 1, constant: 200))
            v.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.height, relatedBy: NSLayoutRelation.greaterThanOrEqual, toItem: nil, attribute: NSLayoutAttribute.notAnAttribute, multiplier: 1, constant: 30))
            
            containerView.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.left, relatedBy: NSLayoutRelation.equal, toItem: containerView, attribute: NSLayoutAttribute.left, multiplier: 1, constant: 10))
            
            containerView.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.right, relatedBy: NSLayoutRelation.equal, toItem: containerView, attribute: NSLayoutAttribute.right, multiplier: 1, constant: 10))
            
            if (last != nil) {
                containerView.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.top, relatedBy: NSLayoutRelation.equal, toItem: last, attribute: NSLayoutAttribute.bottom, multiplier: 1, constant: 10))
            } else {
                containerView.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.top, relatedBy: NSLayoutRelation.equal, toItem: containerView, attribute: NSLayoutAttribute.top, multiplier: 1, constant: 10))
            }
            last = v
        }
        
        containerView.addConstraint(NSLayoutConstraint(item:containerView , attribute: NSLayoutAttribute.bottom, relatedBy: NSLayoutRelation.equal, toItem: last!, attribute: NSLayoutAttribute.bottom, multiplier: 1, constant: 10))
        
        
        
//        containerView.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.top, relatedBy: NSLayoutRelation.equal, toItem: nil, attribute: NSLayoutAttribute.notAnAttribute, multiplier: 1, constant: 10))
//
//        containerView.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.left, relatedBy: NSLayoutRelation.equal, toItem: nil, attribute: NSLayoutAttribute.notAnAttribute, multiplier: 1, constant: 10))
        
        self.accessoryView = containerView
        
        self.layout()
        
        containerView.superview!.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.left, relatedBy: NSLayoutRelation.equal, toItem: containerView.superview, attribute: NSLayoutAttribute.left, multiplier: 1, constant: 10))
        
        containerView.superview!.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.top, relatedBy: NSLayoutRelation.equal, toItem: containerView.superview, attribute: NSLayoutAttribute.top, multiplier: 1, constant: 10))
        
        containerView.superview!.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.bottom, relatedBy: NSLayoutRelation.equal, toItem: containerView.superview, attribute: NSLayoutAttribute.bottom, multiplier: 1, constant: 10))
        
        containerView.superview!.addConstraint(NSLayoutConstraint(item: containerView, attribute: NSLayoutAttribute.right, relatedBy: NSLayoutRelation.equal, toItem: containerView.superview, attribute: NSLayoutAttribute.right, multiplier: 1, constant: 10))
        
        self.layout()
        
        self.addButton(withTitle: "Ok")
        
        validateInputs()
        NotificationCenter.default.addObserver(self, selector: #selector(SessionSetupAlert.validateInputs), name: NSNotification.Name.NSTextDidChange, object: nil)
    }
    
    func validateInputs() {
        let enabled = !localPath.stringValue.isEmpty &&
            !localURL.stringValue.isEmpty && URL(string: localURL.stringValue) != nil && NSURLConnection.canHandle(URLRequest(url: URL(string: localURL.stringValue)!)) &&
            !remoteURL.stringValue.isEmpty && URL(string: remoteURL.stringValue) != nil && NSURLConnection.canHandle(URLRequest(url: URL(string: remoteURL.stringValue)!)) &&
            !remoteUser.stringValue.isEmpty &&
            !remotePassword.stringValue.isEmpty
        
        
        self.buttons.last?.isEnabled = enabled;
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
}
