import SwiftUI

struct LogView: View {
    @ObservedObject var logMessages = LogMessages.shared

    var body: some View {
        VStack(alignment: .leading, spacing: 12.0) {
            GeometryReader { _ in
                ScrollView {
                    VStack(alignment: .leading, spacing: 0) {
                        ForEach(logMessages.entries) { e in
                            Text(e.text)
                                .font(.custom("Menlo", size: 11.0))
                                .foregroundColor(e.foregroundColor)
                                .background(e.backgroundColor)
                        }
                    }
                }
            }
        }
        .padding()
    }
}

struct LogView_Previews: PreviewProvider {
    static var previews: some View {
        LogView()
    }
}