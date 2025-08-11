# ============================================
# Billu Input System (input_system.py)
# ============================================

from rule_based_ai import parse_input, send_command

DEBUG_MODE = True  # Keep True for testing

def main():
    print("🤖 Billu INPUT SYSTEM is ready.")
    print("💬 Type something for Billu (type 'exit' to quit)\n")

    while True:
        user_text = input("🗣 You: ")

        # ✅ Exit condition
        if user_text.lower() in ["exit", "quit"]:
            print("👋 Exiting Billu.")
            break

        # ✅ Pass to NLP brain
        cmds = parse_input(user_text)

        if cmds:
            if DEBUG_MODE:
                print(f"[DEBUG] NLP detected → {cmds}")

            # ✅ Send each command to ESP32
            for cmd in cmds:
                send_command(cmd)
        else:
            print("🤔 Billu didn’t understand that yet.")

if __name__ == "__main__":
    main()
