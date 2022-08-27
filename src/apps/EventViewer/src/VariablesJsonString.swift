import SwiftUI

private func callback(_ filePath: UnsafePointer<Int8>?, _ context: UnsafeMutableRawPointer?) {
  if filePath == nil { return }

  let path = String(cString: filePath!)
  let obj: VariablesJsonString? = unsafeBitCast(context, to: VariablesJsonString.self)

  guard let text = try? String(contentsOfFile: path, encoding: .utf8) else { return }

  DispatchQueue.main.async { [weak obj] in
    guard let obj = obj else { return }

    obj.text = text
  }
}

public class VariablesJsonString: ObservableObject {
  public static let shared = VariablesJsonString()

  @Published var text = ""

  private init() {
    let obj = unsafeBitCast(self, to: UnsafeMutableRawPointer.self)
    libkrbn_enable_manipulator_environment_json_file_monitor(callback, obj)
  }

  deinit {
    libkrbn_disable_manipulator_environment_json_file_monitor()
  }
}
