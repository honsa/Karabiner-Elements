import SwiftUI

public class AppIcon: Identifiable, Equatable {
  public var id: Int32
  public var karabinerElementsThumbnailImage: NSImage?
  public var eventViewerThumbnailImage: NSImage?
  public var multitouchExtensionThumbnailImage: NSImage?

  init(_ id: Int32) {
    self.id = id

    karabinerElementsThumbnailImage = NSImage(
      named: String(format: "%03d-KarabinerElements.png", id))
    eventViewerThumbnailImage = NSImage(
      named: String(format: "%03d-EventViewer.png", id))
    multitouchExtensionThumbnailImage = NSImage(
      named: String(format: "%03d-MultitouchExtension.png", id))
  }

  public static func == (lhs: AppIcon, rhs: AppIcon) -> Bool {
    lhs.id == rhs.id
  }
}

public class AppIcons: ObservableObject {
  public static let shared = AppIcons()

  private var didSetEnabled = false
  // Start GrabberClient when init
  private var grabberClient = LibKrbn.GrabberClient.shared

  @Published var icons: [AppIcon] = []

  @Published var selectedAppIconNumber: Int32 = 0 {
    didSet {
      if didSetEnabled {
        grabberClient.setAppIcon(selectedAppIconNumber)
      }
    }
  }

  init() {
    icons.append(AppIcon(0))
    icons.append(AppIcon(1))
    icons.append(AppIcon(2))

    if let jsonData = try? Data(
      contentsOf: URL(
        fileURLWithPath: String(cString: libkrbn_get_system_app_icon_configuration_file_path())))
    {
      if let jsonDict =
        try? JSONSerialization.jsonObject(with: jsonData, options: []) as? [String: Any]
      {
        if let number = jsonDict["number"] as? Int32 {
          selectedAppIconNumber = number
        }
      }
    }

    didSetEnabled = true
  }
}
