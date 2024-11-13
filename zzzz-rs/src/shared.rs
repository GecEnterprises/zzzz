#[derive(Clone)]
pub struct SharedState {
    pub(crate) volume: f32,
    pub(crate) frequency: f32,
    pub(crate) is_playing: bool
}

pub enum AudioMessage {
    SetVolume(f32),
    SetFrequency(f32),
    Play,
    Stop
}
