import SwiftUI

struct ActivateDriverButton: View {
  @State private var showingProgress: Bool = false
  @State private var showingResult: Bool = false
  @State private var status: Int32 = 0

  var body: some View {
    VStack(alignment: .leading) {
      Button(action: {
        self.showingProgress = true
        self.showingResult = false

        VirtualHIDDeviceManager.shared.activateDriver(completion: { status in
          DispatchQueue.main.async {
            self.status = status
            self.showingProgress = false
            self.showingResult = true
          }
        })
      }) {
        Label(
          "Activate driver",
          systemImage: "star.fill")
      }

      if self.showingProgress {
        Text("Activating ...")
          .padding(.bottom, 20)
      }

      if self.showingResult {
        VStack(alignment: .leading) {
          if self.status == 0 {
            Text("Driver was successfully activated.")
              .bold()
          } else {
            Text("Activation was failed. (error: \(self.status))")
              .bold()
          }
        }.padding(.bottom, 20)
      }
    }
  }
}
