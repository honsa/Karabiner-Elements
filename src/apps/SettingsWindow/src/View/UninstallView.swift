import SwiftUI

struct UninstallView: View {
  var body: some View {
    VStack(alignment: .leading, spacing: 24.0) {
      GroupBox(label: Text("Uninstall")) {
        VStack(alignment: .leading, spacing: 12.0) {
          HStack {
            Button(action: {
              libkrbn_launch_uninstaller()

              NSApplication.shared.terminate(nil)
            }) {
              Label("Launch uninstaller", systemImage: "trash")
                .buttonLabelStyle()
            }
            .deleteButtonStyle()

            Spacer()
          }
        }
        .padding(6.0)
      }

      Spacer()
    }
    .padding()
  }
}

struct UninstallView_Previews: PreviewProvider {
  static var previews: some View {
    UninstallView()
  }
}
