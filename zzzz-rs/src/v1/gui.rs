use std::sync::{Arc, RwLock};
use crossbeam_channel::Sender;
use imgui::Condition;
use crate::shared::{AudioMessage, SharedState};

mod support;

pub fn render_gui(
    audio_state: Arc<RwLock<SharedState>>,
    audio_tx: Sender<AudioMessage>
) {
    let mut value = 0;
    let choices = ["test test this is 1", "test test this is 2"];
    support::simple_init(file!(), move |_, ui| {
        ui.window("Hello world")
            .size([300.0, 160.0], Condition::FirstUseEver)
            .build(|| {
                ui.text_wrapped("Hello world!");
                ui.text_wrapped("こんにちは世界！");
                if ui.button(choices[value]) {
                    value += 1;
                    value %= 2;
                }

                ui.button("This...is...imgui-rs!");
                ui.separator();
                let mouse_pos = ui.io().mouse_pos;

                let freq = audio_state.write().unwrap().frequency;
                let prev_freq = freq as i32;
                let mut ifreq = freq as i32;

                ui.slider(
                    "Frequency",
                    5,
                    1000,
                    &mut ifreq
                );

                if prev_freq != ifreq {
                    audio_tx.send(AudioMessage::SetFrequency(ifreq as f32)).unwrap();
                }

                // audio_tx.send(AudioMessage::SetVolume(mouse_pos[1])).unwrap();

                ui.text(format!(
                    "Mouse Position: ({:.1},{:.1})",
                    mouse_pos[0], mouse_pos[1]
                ));
            });
    });
}